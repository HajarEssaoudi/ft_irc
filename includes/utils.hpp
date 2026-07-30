#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <sstream>

// ── Codes d'erreur IRC ────────────────────────────
#define ERR_UNKNOWNCOMMAND    421
#define ERR_NEEDMOREPARAMS    461
#define ERR_NONICKNAMEGIVEN   431
#define ERR_ERRONEUSNICKNAME  432
#define ERR_NICKNAMEINUSE     433
#define ERR_NOTREGISTERED     451
#define ERR_ALREADYREGISTERED 462
#define ERR_PASSWDMISMATCH    464
#define ERR_NORECIPIENT       411
#define ERR_NOTEXTTOSEND      412
#define ERR_NOSUCHNICK        401

// Channel error codes
#define ERR_NOSUCHCHANNEL     403
#define ERR_USERNOTINCHANNEL  441
#define ERR_NOTONCHANNEL      442
#define ERR_USERONCHANNEL     443
#define ERR_CHANNELISFULL     471
#define ERR_UNKNOWNMODE       472
#define ERR_INVITEONLYCHAN    473
#define ERR_BADCHANNELKEY     475
#define ERR_CHANOPRIVSNEEDED  482
#define ERR_NOORIGIN          409

// ── Codes Replies IRC ────────────────────────────
#define RPL_WHOISUSER   311
#define RPL_ENDOFWHOIS  318

// Channel reply codes
#define RPL_NOTOPIC        331
#define RPL_TOPIC          332
#define RPL_INVITING       341
#define RPL_NAMREPLY       353
#define RPL_ENDOFNAMES     366
#define RPL_CHANNELMODEIS  324


//irssi error

typedef struct s_msg
{
    std::string              prefix;
    std::string              command;
    std::vector<std::string> params;
    std::string              trailing;
} Message;

Message     parseLine(const std::string &line);
std::string toString(int value);

#endif