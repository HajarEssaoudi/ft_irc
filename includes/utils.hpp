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

// ── La struct Message ─────────────────────────────
typedef struct s_msg
{
    std::string              prefix;
    std::string              command;
    std::vector<std::string> params;
    std::string              trailing;
} Message;

// ── Fonctions de parsing ──────────────────────────
Message     parseLine(const std::string &line);
bool        isValidCmd(std::string cmd);
bool        hasEnoughParams(const Message &msg);
std::string toString(int value);

#endif