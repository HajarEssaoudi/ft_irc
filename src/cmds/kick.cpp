#include "../../includes/Server.hpp"

void Server::kickCommand(Client *client, const Message &msg)
{
    // Check inf if client is auth before running join
    if (!client->isAuthenticated())
    {
        raiseError(client->fd, ERR_NOTREGISTERED, "");
        return;
    }

    // Check parameters
    if (msg.params.size() < 2)
    {
        raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
        return;
    }

    std::string channelName = msg.params[0];
    std::string nick = msg.params[1];

    // Find the channel
    Channel *channel = requireChannel(client, channelName);
    if (!channel)
        return;

    // Sender must be a member
    if (!requireMember(client, channel))
        return;

    // Sender must be an operator
    if (!requireOperator(client, channel))
        return;

    // Find the target
    Client *target = requireClient(client, nick);
    if (!target)
        return;

    // Target must be in the channel
    if (!channel->hasMember(target))
    {
        raiseError(client->fd, ERR_USERNOTINCHANNEL, nick + " " + channelName);
        return;
    }

// Build the kick message
    std::string reply = ":" + client->getPrefix() + " KICK " + channelName + " " + nick;

    if (!msg.trailing.empty())
        reply += " :" + msg.trailing;

    channel->broadcast(reply);

    // Remove the user
    channel->removeMember(target);

    if (channel->isOperator(target))
        channel->removeOperator(target);

    // Delete channel if it's empty
    if (channel->empty())
        removeChannel(channelName);
}