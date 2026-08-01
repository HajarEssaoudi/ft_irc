#include "../includes/Server.hpp"

void Server::execBot(Client *client, const std::string &message)
{
    static const char *jokes[] =
    {
        "Why do programmers prefer dark mode? Because light attracts bugs.",
        "There are only 10 kinds of people: those who understand binary and those who don't.",
        "A SQL query walks into a bar and asks: Can I join you?",
        "Debugging is like being the detective in a crime movie where you're also the murderer.",
        "Why did the programmer quit his job? Because he didn't get arrays."
    };

    std::string reply;

    if (message == "help")
        reply = "Commands: joke, help";
    else if (message == "joke")
        reply = jokes[rand() % 5];
    else
        reply = "Unknown command. Try 'joke' or 'help'.";

    client->sendMessage(
        ":JokeBot!bot@localhost PRIVMSG " +
        client->nickname + " :" + reply);
}

void Server::privmsgCommand(Client *client, const Message &msg)
{
    if (!client->authenticated)
    {
        raiseError(client->fd, ERR_NOTREGISTERED, "");
        return;
    }
    if (msg.params.empty())
    {
        raiseError(client->fd, ERR_NORECIPIENT, msg.command);
        return;
    }
    if (msg.trailing.empty())
    {
        raiseError(client->fd, ERR_NOTEXTTOSEND, "");
        return;
    }

    std::string target = msg.params[0];

    if (target[0] == '#')
    {
        Channel *channel = getChannel(target);
        if (!channel)
        {
            raiseError(client->fd, ERR_NOSUCHNICK, target);
            return;
        }
        if (!channel->hasMember(client))
        {
            raiseError(client->fd, ERR_NOTONCHANNEL, target);
            return;
        }
        std::string reply = ":" + client->getPrefix() +
                           " PRIVMSG " + target + " :" + msg.trailing;
        const std::set<Client *> &members = channel->getMembers();
        for (std::set<Client *>::const_iterator it = members.begin();
             it != members.end(); ++it)
        {
            if (*it != client)
                (*it)->sendMessage(reply);
        }
        return;
    }

    if (target == "JokeBot")
    {
        execBot(client, msg.trailing);
        return;
    }

    Client *receiver = getClientByNick(target);
    if (!receiver)
    {
        raiseError(client->fd, ERR_NOSUCHNICK, target);
        return;
    }

    std::string text = msg.trailing;
    if (!text.empty() && text[0] == '\001' &&
        text.find("DCC ") != std::string::npos)
    {
        std::cout << "DCC request detected" << std::endl;
    }

    std::string reply = ":" + client->getPrefix() +
                       " PRIVMSG " + receiver->nickname +
                       " :" + msg.trailing;
    receiver->sendMessage(reply);
}