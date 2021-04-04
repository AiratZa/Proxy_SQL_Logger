#include <signal.h>
#include <iostream>

#include "ProxyServer.hpp"
#include "Config.hpp"

#define SERVER_SUCCESS_STOP_LOG "\n\n¯\\_(ツ)_/¯ Proxy_SQL_Logger is stopped ¯\\_(ツ)_/¯\n\n"


class SuperVisor {
public:
    SuperVisor() : _server(NULL) { };

    void setWebServ(ProxyServer* server) { _server = server; }
    void stopServer(void) { _server->stop(); }

private:
    ProxyServer* _server;
};

SuperVisor supervisor;


void stopSignalHandler(int signal) {
    (void)signal;
    supervisor.stopServer();
    std::cout << SERVER_SUCCESS_STOP_LOG << std::endl;
    exit(EXIT_SUCCESS);
}

std::string ProxyServer::log_filename;


int main(int argc, char *argv[])
{
    if (argc != 7) {
        std::cerr << "Please provide all needed arguments:" << std::endl;
        std::cout << "./proxy_sql_logger --user_side <host:port> --server_side <host:port> --log_file <filename.txt>" << std::endl;
        exit(EXIT_FAILURE);
    }


    signal(SIGINT, stopSignalHandler);
    signal(SIGTERM, stopSignalHandler);

    Config conf = Config(argv);

    ProxyServer::log_filename = conf.getLogFileName();
    ProxyServer server = ProxyServer(&conf);
    supervisor.setWebServ(&server);

    server.serveConnections();

    return 0;
}
