#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include <set>

void Server::nickCommand(Client *client, const Message &msg)
{
    if (!client->hasPassword)
    {
        raiseError(client->fd, ERR_NOTREGISTERED, "");
        return;
    }

    if (msg.params.empty())
    {
        raiseError(client->fd, ERR_NONICKNAMEGIVEN, msg.command);
        return;
    }

    // TODO: Validate nickname
    if (!isValidNickname(msg.params[0]))
    {
        raiseError(client->fd, ERR_ERRONEUSNICKNAME, msg.params[0]);
        return;
    }
    

    // Check if nickname is already in use
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        if (it->second != client && it->second->nickname == msg.params[0])
        {
            raiseError(client->fd, ERR_NICKNAMEINUSE, msg.params[0]);
            return;
        }
    }

    bool changingNick = client->hasNick;
    std::string oldNick = client->nickname;

    client->nickname = msg.params[0];
    client->hasNick = true;

    if (changingNick)
    {
        std::string nickMsg = ":" + oldNick + "!" + client->username + "@localhost NICK :" + client->nickname;

        std::set<Client*> sent;

        // // Send to the client itself
        // client->sendMessage(nickMsg);
        // sent.insert(client);

        /*if the client change his name, a msg will appear to all the clients that share with him at least one channel*/
        for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        {
            Channel *channel = it->second;

            if (!channel->hasMember(client))
                continue;

            const std::set<Client*> &members = channel->getMembers();

            for (std::set<Client*>::const_iterator m = members.begin();
                 m != members.end(); ++m)
            {
                if (sent.insert(*m).second)
                    (*m)->sendMessage(nickMsg);
            }
        }
    }

    tryAuthenticate(client->fd);
}
