#include "../../includes/Server.hpp"

/// Helper functions ///
Channel* Server::requireChannel(Client *client, const std::string &name)
{
    Channel *channel = getChannel(name);

    if (!channel)
    {
        client->sendMessage("403 " + name + " :No such channel\r\n");
    }
    return channel;
}

bool Server::requireMember(Client *client, Channel *channel)
{
    if (!channel->hasMember(client))
    {
        client->sendMessage("442 " +
                            channel->getName() +
                            " :You're not on that channel\r\n");
        return false;
    }
    return true;
}

bool Server::requireOperator(Client *client, Channel *channel)
{
    if (!channel->isOperator(client))
    {
        client->sendMessage(":ircserv 482 " + client->nickname + " " + channel->getName() + " :You're not channel operator\r\n");
        return false;
    }
    return true;
}

Client* Server::requireClient(Client *requester, const std::string &nickname)
{
    Client *client = getClientByNick(nickname);

    if (!client)
    {
        requester->sendMessage("401 " + nickname + " :No such nick\r\n");
    }
    return client;
}
