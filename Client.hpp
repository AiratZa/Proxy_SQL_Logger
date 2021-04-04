//
// Created by Great Drake on 4/3/21.
//

#ifndef PROXY_SQL_CLIENT_HPP
#define PROXY_SQL_CLIENT_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>

#include "utils.hpp"
class Client;
#include "ProxyServer.hpp"

#define BUFFER_LENGHT 1024

#define RECV_CLIENT_REQ 1
#define SEND_CLIENT_REQ 2
#define RECV_SERVER_RESP 3
#define SEND_SERVER_RESP 4


class Client {
public:

    Client() {};
    Client(int socket):
        _socket(socket),
        _server_conn_socket(-1),
        _status(RECV_CLIENT_REQ),
        _bytes_read(0),
        _client_raw_request_sent_len(0),
        _client_raw_request_remains(0),
        _server_raw_response_sent_len(0),
        _server_raw_response_remains(0),
        _is_done(false),
        _message_len(0)
    {
        setConnectionToDB();
    };

    ~Client()
    {
        close(_socket);
        close(_server_conn_socket);
    }

    void    setConnectionToDB(void);

    int getSocket(void) const { return _socket; }

    int getServerConnSocket(void) const { return _server_conn_socket; }

    void setStatus(int status) { _status = status; }
    int getStatus(void) const { return _status; }


    int recvClientRequest(void);

    int recvSQLServerResponse(void);

    int sendClientRequestToSQLServer(void);

    int sendSQLServerResponseToClient(void);

    bool isDone() {return _is_done; }

    void setDone() { _is_done = true; }


private:
    void checkAndSetQueryMessageLen(void);

    int _socket;

    int _server_conn_socket;
    int _status;

    char    _buf[BUFFER_LENGHT];
    int     _bytes_read;

    std::string _client_raw_request;
    long _client_raw_request_sent_len;
    long _client_raw_request_remains;

    std::string _server_raw_response;
    long _server_raw_response_sent_len;
    long _server_raw_response_remains;

    bool _is_done;

    int _message_len;
};

#endif //PROXY_SQL_CLIENT_HPP
