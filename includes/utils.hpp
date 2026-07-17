#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

typedef struct s_msg
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string trailing;
} Message;

Message parseLine(const std::string &line);

#endif
