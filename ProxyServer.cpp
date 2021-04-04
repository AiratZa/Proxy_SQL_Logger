//
// Created by Great Drake on 4/3/21.
//

#include "ProxyServer.hpp"

ProxyServer::~ProxyServer(void) {
    stop();
}

void ProxyServer::stop() {
    for(std::list<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* client = *it;
        close(client->getSocket());
        close(client->getServerConnSocket());
    }
}


ProxyServer::ProxyServer(Config* conf)
    : _conf(conf) {
    _client_listener = socket(AF_INET, SOCK_STREAM, 0);
    if(_client_listener < 0)
        utils::exitWithLog("socket");

    if (fcntl(_client_listener, F_SETFL, O_NONBLOCK) < 0) //превращает сокет в неблокирующий
        utils::exitWithLog("fcntl");

    int optval = 1;
    if (setsockopt(_client_listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        utils::exitWithLog("setsockopt");

    const IpPort& user_side = _conf->getUserSideParams();

    struct sockaddr_in _client_addr;
    _client_addr.sin_family = AF_INET;
    _client_addr.sin_port = htons(user_side.port);
    _client_addr.sin_addr.s_addr = user_side.host;

    if (bind(_client_listener, (struct sockaddr *)&_client_addr, sizeof(_client_addr)) < 0)
        utils::exitWithLog("bind");

    if (listen(_client_listener, SOMAXCONN) < 0)
        utils::exitWithLog("listen");

    std::cout << "ProxyServer is started" << std::endl;
}


void ProxyServer::fillFDSets(fd_set* temp_read_set, fd_set* temp_write_set)
{
    std::list<int> tmp;

    tmp.push_back(_client_listener);
    FD_SET(_client_listener, temp_read_set);

    std::list<Client*>::iterator it = _clients.begin();

    while (it != _clients.end())
    {
        Client* client = *it;
        int status = client->getStatus();

        int socket_fd = 0;

        if (client->isDone()) {
            delete client;
            it = _clients.erase(it);
            continue;
        }

        if (status == RECV_CLIENT_REQ) {
            socket_fd = client->getSocket();
            FD_SET(socket_fd, temp_read_set);
        }
        else if (status == RECV_SERVER_RESP) {
            socket_fd = client->getServerConnSocket();
            FD_SET(socket_fd, temp_read_set);
        }
        else if (status == SEND_SERVER_RESP) {
            socket_fd = client->getSocket();
            FD_SET(socket_fd, temp_write_set);
        }
        else if (status == SEND_CLIENT_REQ) {
            socket_fd = client->getServerConnSocket();
            FD_SET(socket_fd, temp_write_set);
        }

        if (socket_fd != 0)
            tmp.push_back(socket_fd);
        ++it;
    }

    _max_fd = *std::max_element(tmp.begin(), tmp.end());
}

void ProxyServer::serveConnections() {
    fd_set temp_read_set;
    fd_set temp_write_set;

    while(true) {

        FD_ZERO(&temp_read_set);
        FD_ZERO(&temp_write_set);

        fillFDSets(&temp_read_set, &temp_write_set);

        // Ждём события в одном из сокетов
        if (select(_max_fd + 1,
                   &temp_read_set,
                   &temp_write_set,
                   NULL,
                   NULL) < 0) {
            utils::exitWithLog("select");
        }

        // Определяем тип события и выполняем соответствующие действия
        if (FD_ISSET(_client_listener, &temp_read_set)) {
            // Поступил новый запрос на соединение, используем accept
            acceptConnection();
        }

        processConnections(&temp_read_set, &temp_write_set);
    }
}


void ProxyServer::acceptConnection(void) {
    struct sockaddr_in _remote_addr;

    socklen_t len = sizeof(_remote_addr);
    int sock = accept(_client_listener, (struct sockaddr *)&_remote_addr, &len);
    if(sock < 0)
        utils::exitWithLog("accept");

    if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
        utils::exitWithLog("fcntl");

    int optval = 1;
    if (setsockopt(_client_listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        utils::exitWithLog("setsockopt");

    _clients.push_back(new Client(sock));
}


int ProxyServer::recvOperations(Client* client, int client_status)
{
    if (client_status == RECV_CLIENT_REQ)
    {
        int ret = client->recvClientRequest();

        if (ret == 0)
            client->setStatus(SEND_CLIENT_REQ);
        else if (ret == -1)
            return -1;
    }
    else // RECV_SERVER_RESP
    {
        int ret = client->recvSQLServerResponse();

        if (ret == 0)
            client->setStatus(SEND_SERVER_RESP);
        else
            return -1;
    }
    return 0;
}

void ProxyServer::sendOperations(Client* client, int client_status)
{
    if (client_status == SEND_CLIENT_REQ)
    {
        int ret = client->sendClientRequestToSQLServer();
        if (ret == 0)
        {
            client->setStatus(RECV_SERVER_RESP);
        }
        else if (ret == -1)
        {
            client->setDone();
            writeLog("ERROR OCCURED WHEN SEND CLIENT REQUEST TO SQL SERVER");
        }
    }
    else // SEND_SERVER_RESP
    {
        int ret = client->sendSQLServerResponseToClient();
        if (ret == 0)
        {
            client->setStatus(RECV_CLIENT_REQ);
        }
        else if (ret == -1)
        {
            client->setDone();
            writeLog("ERROR OCCURED WHEN SEND SQL SERVER RESPONSE TO CLIENT");
        }
    }
}

void ProxyServer::processConnections(fd_set* globalReadSetPtr, fd_set* globalWriteSetPtr) {

    std::list<Client*>::iterator it = _clients.begin();

    while (it != _clients.end()) {
        int client_sock = (*it)->getSocket();
        int server_con_socket = (*it)->getServerConnSocket();
        int client_status = (*it)->getStatus();

        if (FD_ISSET(client_sock, globalReadSetPtr) || FD_ISSET(server_con_socket, globalReadSetPtr))
        {
            if (recvOperations(*it, client_status) == -1) // recv return -1, connection closed
            {
                delete (*it);
                it = _clients.erase(it);
                writeLog("Couldn't read from socket, connection closed");
                continue;
            }
        }
        else if (FD_ISSET(client_sock, globalWriteSetPtr) || FD_ISSET(server_con_socket, globalWriteSetPtr))
        {
            sendOperations(*it, client_status);
            if ((*it)->isDone())
            {
                delete (*it);
                it = _clients.erase(it);
                continue;
            }

        }
        ++it;
    }

}
