#ifndef PARSING_HPP
#define PARSING_HPP

#include "Client.hpp"
#include <string>
#include <vector>
#include <sstream>


#define ERR_UNKNOWNCOMMAND 421
#define ERR_NEEDMOREPARAMS 461
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NOTREGISTERED 451
#define ERR_ALREADYREGISTERED 462
#define ERR_PASSWDMISMATCH 464
// :localhost CODE <params>  
typedef struct s_msg
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string trailing;
} Message;

Message parseLine(const std::string &line);
/*utils*/
bool    isValidCmd(std::string cmd);

/*Error*/
// void raiseError(Client &client, int code, const std::string &arg);
bool hasEnoughParams(const Message &msg);

#endif
