//
// Created by Great Drake on 4/3/21.
//

#ifndef PROXY_SQL_UTILS_HPP
#define PROXY_SQL_UTILS_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include <cerrno> //errno
#include <cstring> //for strerror
#include <stdlib.h>
#include <cctype>


#define QUERY_TYPE 'Q'
#define OTHER_TYPE 'O'

struct IpPort
{
    IpPort(void) : port(0) { };

    IpPort(const std::string& host_str, int port)
            : host_str(host_str), port(port)
    {
        host = inet_addr(host_str.c_str());
    }

    in_addr_t host;
    std::string host_str;
    int port;
};


namespace utils
{
    char    getPGMessageType(const std::string& raw_message);
    void    exitWithLog(const std::string& log);

    std::string strip(const std::string &str);
    bool is_correct_ip_octet(const std::string& octet_str);

    int unsigned_number_len(unsigned int nbr);
}

#endif //PROXY_SQL_UTILS_HPP
