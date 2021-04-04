//
// Created by airat on 04.04.2021.
//

#ifndef PROXY_SQL_CONFIG_HPP
#define PROXY_SQL_CONFIG_HPP

#include <list>

#include <fstream>
#include <iostream>

#include "utils.hpp"

#define USER_SIDE_HOST_PORT "--user_side"
#define SERVER_SIDE_HOST_PORT "--server_side"
#define LOG_FILENAME "--log_file"

class Config
{
public:
    Config(char *argv[]);

    const IpPort& getUserSideParams(void) const { return _user_side; }
    const IpPort& getServerSideParams(void) const { return _server_side; }

    const std::string& getLogFileName(void) const { return _log_filename; }

private:
    void parseParam(const std::string& param_name, const std::string& value);
    void parseHostPort(const std::string& param_name, const std::string& value);

    const std::string parseHost(const std::string& param) const;
    int parsePort(const std::string& param) const;

    IpPort _user_side;
    IpPort _server_side;
    std::string _log_filename;

};


#endif //PROXY_SQL_CONFIG_HPP
