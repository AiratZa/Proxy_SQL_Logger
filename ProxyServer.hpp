//
// Created by Great Drake on 4/3/21.
//

#ifndef PROXY_SQL_PROXYSERVER_HPP
#define PROXY_SQL_PROXYSERVER_HPP

class ProxyServer;
#include <algorithm>
#include <vector>
#include <list>
#include <map>

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fcntl.h>

#include "utils.hpp"
#include "Client.hpp"
#include "Config.hpp"


class ProxyServer {
private:
    Config* _conf;
    int _client_listener;
    int _max_fd;

    std::list<Client*> _clients;

    void acceptConnection(void);
    void processConnections(fd_set* globalReadSetPtr, fd_set* globalWriteSetPtr);

    int recvOperations(Client* client, int client_status);
    void sendOperations(Client* client, int client_status);


public:
    ProxyServer(void) {};
    ~ProxyServer(void);

    ProxyServer(Config* conf);

    void fillFDSets(fd_set* temp_read_set, fd_set* temp_write_set);

    void    serveConnections();
    void    stop();

    static std::string log_filename;

    static void writeLog(const std::string& log_text)
    {
        std::ofstream outfile(log_filename.c_str(), std::ios::app);

        if (!outfile)
            utils::exitWithLog("Error occured when open log file");
        outfile << std::endl << "########################################################################################################################" << std::endl;
        outfile << log_text << std::endl;
        outfile << std::endl << "########################################################################################################################" << std::endl;

    }
};


#endif //PROXY_SQL_PROXYSERVER_HPP
