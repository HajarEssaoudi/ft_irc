#include "../includes/utils.hpp"

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

    msg.command = line.substr(0, pos);

    if (pos == std::string::npos)
        line.clear();
    else
        line = line.substr(pos + 1);
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
