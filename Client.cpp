//
// Created by Great Drake on 4/3/21.
//

#include "Client.hpp"

void    Client::setConnectionToDB(void)
{
    struct sockaddr_in addr;

    _server_conn_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_server_conn_socket < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(5432); // или любой другой порт...
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(connect(_server_conn_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
}


int Client::recvClientRequest(void)
{
    _bytes_read = recv(_socket, _buf, BUFFER_LENGHT - 1, 0);

    if (_bytes_read < 0) { // Соединение разорвано, удаляем сокет из множества //
        return -1;
    }

    _buf[_bytes_read] = '\0';

    _client_raw_request.append(_buf, _bytes_read);

    // TEMPORARY SOLUTION
    // Because of lack of time to parse all types of messages
    if (utils::getPGMessageType(_client_raw_request) == QUERY_TYPE)
        checkAndSetQueryMessageLen();

    if ((_bytes_read == 0)
|| ( (utils::getPGMessageType(_client_raw_request) != QUERY_TYPE) && _client_raw_request.size() < (BUFFER_LENGHT - 1) )
|| ( (utils::getPGMessageType(_client_raw_request) == QUERY_TYPE) && ((_client_raw_request.size() - 1) == static_cast<long unsigned int>(_message_len) ) ))  {

        _client_raw_request_remains = _client_raw_request.size();

        if (_client_raw_request_remains && (utils::getPGMessageType(_client_raw_request) == QUERY_TYPE))
        {
            ProxyServer::writeLog(_client_raw_request.substr(5, _client_raw_request.size()-6));
        }
        return 0;
    }
    return 1;
}

int Client::recvSQLServerResponse(void)
{
    _bytes_read = recv(_server_conn_socket, _buf, BUFFER_LENGHT - 1, 0);

    if (_bytes_read < 0) { // Соединение разорвано, удаляем сокет из множества //
        return -1;
    }

    _buf[_bytes_read] = '\0';

    _server_raw_response.append(_buf, _bytes_read);

    if ((_bytes_read == 0) || _bytes_read < (BUFFER_LENGHT - 1)) {
        _server_raw_response_remains = _server_raw_response.size();
        return 0;
    }
    return 1;
}


int Client::sendClientRequestToSQLServer(void)
{
    long ret = 0;
    ret = send(_server_conn_socket, _client_raw_request.c_str() + _client_raw_request_sent_len, _client_raw_request_remains, 0);
    if (ret >= 0) {
        _client_raw_request_sent_len += ret;
        _client_raw_request_remains -= ret;
        if (_client_raw_request_remains == 0) {
            _client_raw_request_sent_len = 0;
            _client_raw_request.clear();
            return 0;
        }
    } else if (ret == -1) {
        return -1;
    }
    return 1;
}

int Client::sendSQLServerResponseToClient(void)
{
    long ret = 0;
    ret = send(_socket, _server_raw_response.c_str() + _server_raw_response_sent_len, _server_raw_response_remains, 0);
    if (ret >= 0) {
        _server_raw_response_sent_len += ret;
        _server_raw_response_remains -= ret;
        if (_server_raw_response_remains == 0) {
            _message_len = 0;
            _server_raw_response_sent_len = 0;
            _server_raw_response.clear();
            return 0;
        }
    } else if (ret == -1) {
        return -1;
    }
    return 1;
}


void Client::checkAndSetQueryMessageLen(void)
{
    if (_client_raw_request.length() >= 5)
    {
        _message_len = int((unsigned char)(_client_raw_request[1]) << 24 |
                    (unsigned char)(_client_raw_request[2]) << 16 |
                    (unsigned char)(_client_raw_request[3]) << 8 |
                    (unsigned char)(_client_raw_request[4]));
    }
}
