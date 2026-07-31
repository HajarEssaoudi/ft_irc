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
        case ERR_UNKNOWNCOMMAND:    msg += arg + " :Unknown command";                break;
        case ERR_NEEDMOREPARAMS:    msg += arg + " :Not enough parameters";          break;
        case ERR_NONICKNAMEGIVEN:   msg += ":No nickname given";                     break;
        case ERR_NICKNAMEINUSE:     msg += arg + " :Nickname is already in use";     break;
        case ERR_ALREADYREGISTERED: msg += ":You may not reregister";                break;
        case ERR_PASSWDMISMATCH:    msg += ":Password incorrect";                    break;
        case ERR_NOTREGISTERED:     msg += ":You have not registered";               break;
        case ERR_NORECIPIENT:       msg += ":No recipient given (" + arg + ")";      break;
        case ERR_NOTEXTTOSEND:      msg += ":No text to send";                       break;
        case ERR_NOSUCHNICK:        msg += arg + " :No such nick/channel";           break;
        case ERR_NOORIGIN:          msg += ":No origin specified";                   break;

        // Channel errors
        case ERR_NOSUCHCHANNEL:     msg += arg + " :No such channel";                break;
        case ERR_USERNOTINCHANNEL:  msg += arg + " :They aren't on that channel";    break;
        case ERR_NOTONCHANNEL:      msg += arg + " :You're not on that channel";     break;
        case ERR_USERONCHANNEL:     msg += arg + " :is already on channel";          break;
        case ERR_CHANNELISFULL:     msg += arg + " :Cannot join channel (+l)";       break;
        case ERR_UNKNOWNMODE:       msg += arg + " :is unknown mode char to me";     break;
        case ERR_INVITEONLYCHAN:    msg += arg + " :Cannot join channel (+i)";       break;
        case ERR_BADCHANNELKEY:     msg += arg + " :Cannot join channel (+k)";       break;
        case ERR_CHANOPRIVSNEEDED:  msg += arg + " :You're not channel operator";    break;
        case ERR_ERRONEUSNICKNAME:  msg += arg + " :Erroneous nickname";             break;

        default:
            return;
    }

    client->sendMessage(msg);
}

void Server::raiseReply(int fd, int code, const std::string &arg1, const std::string &arg2)
{
    Client *client = _clients[fd];
    std::string msg = ":ircserv " + toString(code) + " ";

    if (client->nickname.empty() || client->nickname == "*")
        msg += "* ";
    else
        msg += client->nickname + " ";

    switch (code)
    {
        case RPL_WHOISUSER:         msg += arg1 + " " + arg2;                       break;
        case RPL_ENDOFWHOIS:        msg += arg1 + " :End of WHOIS list";            break;
        //channels
        case RPL_CHANNELMODEIS:     msg += arg1 + " " + arg2;                       break;
        case RPL_NOTOPIC:           msg += arg1 + " :No topic is set";              break;
        case RPL_TOPIC:             msg += arg1 + " :" + arg2;                      break;
        case RPL_INVITING:          msg += arg1;                                    break;
        case RPL_NAMREPLY:          msg += "= " + arg1 + " :" + arg2;               break;
        case RPL_ENDOFNAMES:        msg += arg1 + " :End of /NAMES list";           break;

        default:
            return;
    }

    client->sendMessage(msg);
}

bool Server::isValidNickname(const std::string &nick)
{
    if (nick.empty())
        return false;

    if (std::isdigit(nick[0]))
        return false;

    for (size_t i = 0; i < nick.size(); i++)
    {
        char c = nick[i];

        if (!std::isalnum(c) &&
            c != '-' &&
            c != '_' &&
            c != '[' &&
            c != ']' &&
            c != '\\' &&
            c != '{' &&
            c != '}' &&
            c != '|')
            return false;
    }

    return true;
}
