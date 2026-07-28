#include "../../includes/Server.hpp"

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

        std::string reply = ":" + client->getPrefix() + " PRIVMSG " + target + " :" + msg.trailing;

        const std::set<Client *> &members = channel->getMembers();

        for (std::set<Client *>::const_iterator it = members.begin();
             it != members.end();
             ++it)
        {
            if (*it != client)
                (*it)->sendMessage(reply);
        }

        return;
    }

    Client *receiver = getClientByNick(target);

    if (!receiver)
    {
        std::cout << "Receiver NOT FOUND" << std::endl;
        raiseError(client->fd, ERR_NOSUCHNICK, target);
        return;
    }

    std::cout << "Receiver FOUND: " << receiver->nickname
              << " fd=" << receiver->fd << std::endl;

    std::string reply = ":" + client->getPrefix() + " PRIVMSG " + receiver->nickname + " :" + msg.trailing;

    std::cout << "Sending: [" << reply << "]" << std::endl;

    receiver->sendMessage(reply);
}