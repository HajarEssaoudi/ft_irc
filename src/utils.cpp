#include "../includes/utils.hpp"
#include "../includes/Server.hpp"


void parsePrefix(Message &msg, std::string &line)
{
    if (!line.empty() && line[0] == ':')
    {
        size_t pos = line.find(' ');
        if (pos == std::string::npos)
        {
            msg.prefix = line.substr(1);
            line.clear();
            return;
        }
        msg.prefix = line.substr(1, pos - 1);
        line = line.substr(pos + 1);
    }
}

void parseCmd(Message &msg, std::string &line)
{
    while (!line.empty() && line[0] == ' ')
        line.erase(0, 1);

    size_t pos = line.find(' ');
    std::string cmd;

    if (pos == std::string::npos)
    {
        cmd = line;
        line.clear();
    }
    else
    {
        cmd = line.substr(0, pos);
        line.erase(0, pos + 1);
    }

    for (size_t i = 0; i < cmd.size(); i++)
        cmd[i] = std::toupper(cmd[i]);

    msg.command = cmd;
}

void parseParameters(Message &msg, std::string &line)
{
    while (!line.empty())
    {
        while (!line.empty() && line[0] == ' ')
            line.erase(0, 1);

        if (line.empty() || line[0] == ':')
            return;

        size_t pos = line.find(' ');
        if (pos == std::string::npos)
        {
            msg.params.push_back(line);
            line.clear();
            return;
        }
        msg.params.push_back(line.substr(0, pos));
        line = line.substr(pos + 1);
    }
}

void parseTrailing(Message &msg, std::string &line)
{
    while (!line.empty() && line[0] == ' ')
        line.erase(0, 1);

    if (!line.empty() && line[0] == ':')
        msg.trailing = line.substr(1);
}

Message parseLine(const std::string &rawline)
{
    Message msg;
    std::string line = rawline;

    parsePrefix(msg, line);
    parseCmd(msg, line);
    parseParameters(msg, line);
    parseTrailing(msg, line);

    return msg;
}


bool isValidCmd(std::string cmd)
{
    if (cmd == "PASS"   || cmd == "NICK"    || cmd == "USER"  ||
        cmd == "JOIN"   || cmd == "PRIVMSG" || cmd == "KICK"  ||
        cmd == "MODE"   || cmd == "INVITE"  || cmd == "TOPIC")
        return true;
    return false;
}

bool hasEnoughParams(const Message &msg)
{
    if (msg.command == "PASS")    return msg.params.size() >= 1;
    if (msg.command == "NICK")    return msg.params.size() >= 1;
    if (msg.command == "USER")    return msg.params.size() >= 3 && !msg.trailing.empty();
    if (msg.command == "JOIN")    return msg.params.size() >= 1;
    if (msg.command == "PRIVMSG") return msg.params.size() >= 1 && !msg.trailing.empty();
    if (msg.command == "KICK")    return msg.params.size() >= 2;
    if (msg.command == "INVITE")  return msg.params.size() >= 2;
    if (msg.command == "TOPIC")   return msg.params.size() >= 1;
    if (msg.command == "MODE")    return msg.params.size() >= 2;
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
        case ERR_UNKNOWNCOMMAND:    msg += arg + " :Unknown command";        break;
        case ERR_NEEDMOREPARAMS:    msg += arg + " :Not enough parameters";  break;
        case ERR_NONICKNAMEGIVEN:   msg += ":No nickname given";             break;
        case ERR_NICKNAMEINUSE:     msg += arg + " :Nickname is already in use"; break;
        case ERR_ALREADYREGISTERED: msg += ":You may not reregister";        break;
        case ERR_PASSWDMISMATCH:    msg += ":Password incorrect";            break;
        case ERR_NOTREGISTERED:    msg += ":You have not registered";            break;
        default: return;
    }
    client->sendMessage(msg);
}