//
// Created by Great Drake on 4/3/21.
//

#include "utils.hpp"

namespace utils
{
    // Query format (Frontend) - 'Q' at start
    // src: https://www.postgresql.org/docs/9.5/protocol-message-formats.html
    char    getPGMessageType(const std::string& raw_message)
    {
        if (raw_message[0] == 'Q')
            return QUERY_TYPE;
        else
            return OTHER_TYPE;
    }


    void exitWithLog(const std::string& log) {
        if (errno)
        {
            std::cerr << log << ": " << std::strerror(errno) << std::endl;
            exit(errno);
        }
        std::cerr << log << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string strip(const std::string &str)
    {
        std::string::const_iterator it = str.begin();
        std::string::const_reverse_iterator end_it = str.rbegin();
        while (std::isspace(*it))
            ++it;
        while (std::isspace(*end_it))
            ++end_it;
        return std::string(it, end_it.base());
    }

    bool is_correct_ip_octet(const std::string& octet_str) {
        size_t len = octet_str.length();
        if ( len <=0 || len > 3 ) {
            return false;
        }
        int nbr = atoi(octet_str.c_str());
        if (nbr < 0 || nbr > 255) {
            return false;
        }

        if (static_cast<int>(len) != unsigned_number_len(nbr))
            return false;

        return true;
    }

    int unsigned_number_len(unsigned int nbr)
    {
        unsigned int number_of_digits = 0;

        do {
            ++number_of_digits;
            nbr /= 10;
        } while (nbr);
        return number_of_digits;
    }

}

