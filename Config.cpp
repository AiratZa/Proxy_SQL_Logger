//
// Created by airat on 04.04.2021.
//

#include "Config.hpp"

Config::Config(char *argv[])
{
    for (int i = 1; i < 4; i++)
    {
        parseParam(std::string(argv[i*2-1]), std::string(argv[i*2]));
    }
}


void Config::parseParam(const std::string& param_name, const std::string& value)
{
    if (param_name == USER_SIDE_HOST_PORT)
    {
        if (_user_side.port)
            utils::exitWithLog(param_name + " is duplicated");
    }
    else if (param_name == SERVER_SIDE_HOST_PORT)
    {
        if (_server_side.port)
            utils::exitWithLog(param_name + " is duplicated");
    }
    else if (param_name == LOG_FILENAME)
    {
        if (_log_filename.size())
            utils::exitWithLog(param_name + " is duplicated");
        _log_filename = value;

        std::ofstream outfile(_log_filename.c_str(), std::ios::app);
        if (!outfile)
            utils::exitWithLog("Error occured when open log file");
        return ;
    }
    else
    {
        utils::exitWithLog("Unknown param is provided: " + param_name);
    }

    parseHostPort(param_name, value);
}


const std::string Config::parseHost(const std::string& param) const {
    const std::string localhost_kw = "localhost";


    std::size_t found_pos = param.find(localhost_kw);
    if (found_pos != std::string::npos) {
        if (found_pos != 0) {
            utils::exitWithLog("host not found in \"" + param);
        }
        return "localhost";
    } else {
        if (param.length() > std::string("255.255.255.255:65536").length()) {
            utils::exitWithLog("host:port combination is not correct in \"" + param);
        }

        std::string tmp_param = param;
        size_t pos = 0;
        std::string octet;

        std::list<std::string> octets;

        while ((pos = tmp_param.find('.')) != std::string::npos) {
            octet = tmp_param.substr(0, pos);
            octets.push_back(octet);
            tmp_param.erase(0, pos + 1);
        }
        if ((pos=tmp_param.find(':')) != std::string::npos) {
            octet = tmp_param.substr(0, pos);
            octets.push_back(octet);
        } else {
            octet = tmp_param.substr(0);
            octets.push_back(octet);
        }

        if (octets.size() != 4) {
            utils::exitWithLog("there is found not equal to 4 delimeted by '.' octets."
                            "it doesnt look like correct ip address: \"" + param);
        }

        std::size_t ip_len_to_return = 3; //3 dots between octets
        std::list<std::string>::iterator it = octets.begin();
        while (it != octets.end()) {
            if (!utils::is_correct_ip_octet(*it)) {
                utils::exitWithLog("one of octet in host ip is not correct: \"" + param);
            }
            ip_len_to_return += (*it).size();
            ++it;
        }
        return param.substr(0, ip_len_to_return);
    }
}



int Config::parsePort(const std::string& param) const {
    size_t len = param.length();

    std::string error_log = "port is not found where it should be: \"" + param;

    if ( len <=0 || len > 5 ) {
        utils::exitWithLog(error_log);
    }
    int port = atoi(param.c_str());

    if (port <= 0 || port > 65535) {
        utils::exitWithLog(error_log);
    }

    if (static_cast<int>(len) != utils::unsigned_number_len(port)) {
        utils::exitWithLog(error_log);
    }

    return port;
}


void Config::parseHostPort(const std::string& param_name, const std::string& value)
{
    std::string err_log = "host:port is not correct";

    std::string host = parseHost(value) ;
    if (value[host.length()] != ':') {
        utils::exitWithLog(err_log);
    }

    std::string tmp_word2 = value.substr(host.length() + 1); // pass ':'
    int port = parsePort(tmp_word2);
    if (value.length() != (utils::unsigned_number_len(port) + 1 + host.length()) ) {
        utils::exitWithLog(err_log);
    }

    if (host == "localhost") {
        host = "127.0.0.1";
    }

    if (param_name == USER_SIDE_HOST_PORT)
    {
        _user_side = IpPort(host, port);
    }
    else if (param_name == SERVER_SIDE_HOST_PORT)
    {
        _server_side = IpPort(host, port);
    }
}
