#include "../includes/parsing.hpp"
#include "../includes/Server.hpp"

bool    isValidCmd(std::string cmd)
{
    if (cmd == "PASS"  || cmd == "NICK"   || cmd == "USER"   || 
        cmd == "JOIN"  || cmd == "PRIVMSG"|| cmd == "KICK"   || 
        cmd == "MODE"  || cmd == "INVITE" || cmd == "TOPIC")
    {
        return true;
    }
    return false;
}

bool hasEnoughParams(const Message &msg)
{
    if (msg.command == "PASS")
        return msg.params.size() >= 1;

    if (msg.command == "NICK")
        return msg.params.size() >= 1;

    if (msg.command == "USER")
        return msg.params.size() >= 3 && !msg.trailing.empty();

    if (msg.command == "JOIN")
        return msg.params.size() >= 1;

    if (msg.command == "PRIVMSG")
        return msg.params.size() >= 1 && !msg.trailing.empty();

    if (msg.command == "KICK")
        return msg.params.size() >= 2;

    if (msg.command == "INVITE")
        return msg.params.size() >= 2;

    if (msg.command == "TOPIC")
        return msg.params.size() >= 1;

    if (msg.command == "MODE")
        return msg.params.size() >= 2;

    return true;
}

std::string toString(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void Server::raiseError(int fd, int code, const std::string &arg)
{
    Client *client = _clients[fd];

    std::string msg = ":ircserv " + toString(code) + " ";

    if (client->nickname.empty() || client->nickname == "*")
        msg += "* ";
    else
        msg += client->nickname + " ";

    switch (code)
    {
        case ERR_UNKNOWNCOMMAND:
            msg += arg + " :Unknown command";
            break;

        case ERR_NEEDMOREPARAMS:
            msg += arg + " :Not enough parameters";
            break;

        case ERR_NONICKNAMEGIVEN:
            msg += ":No nickname given";
            break;

        case ERR_NICKNAMEINUSE:
            msg += arg + " :Nickname is already in use";
            break;

        case ERR_ALREADYREGISTERED:
            msg += ":You may not reregister";
            break;

        case ERR_PASSWDMISMATCH:
            msg += ":Password incorrect";
            break;

        default:
            return;
    }

    client->sendMessage(msg);
}
