#include "../../includes/Server.hpp"

/// Helper functions ///
Channel* Server::requireChannel(Client *client, const std::string &name)
{
    Channel *channel = getChannel(name);

    if (!channel)
        raiseError(client->fd, ERR_NOSUCHCHANNEL, name);

    return channel;
}

bool Server::requireMember(Client *client, Channel *channel)
{
    if (!channel->hasMember(client))
    {
        raiseError(client->fd, ERR_NOTONCHANNEL, channel->getName());
        return false;
    }
    return true;
}

bool Server::requireOperator(Client *client, Channel *channel)
{
    if (!channel->isOperator(client))
    {
        raiseError(client->fd, ERR_CHANOPRIVSNEEDED, channel->getName());
        return false;
    }
    return true;
}

Client* Server::requireClient(Client *requester, const std::string &nickname)
{
    Client *client = getClientByNick(nickname);

    if (!client)
        raiseError(requester->fd, ERR_NOSUCHNICK, nickname);

    return client;
}
