#include "../../includes/Server.hpp"

void Server::inviteCommand(Client *client, const Message &msg)
{
    // Checking if client is auth
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

    std::string nick = msg.params[0];
    std::string channelName = msg.params[1];

    // Target client
    Client *target = requireClient(client, nick);
    if (!target)
        return;

    // Find the channel
    Channel *channel = requireChannel(client, channelName);
    if (!channel)
        return;

    // Inviter must be a member
    if (!requireMember(client, channel))
        return;

    // Inviter must be an operator
    if (!requireOperator(client, channel))
        return;

    // Target already in channel?
    if (channel->hasMember(target))
    {
        raiseError(client->fd, ERR_USERONCHANNEL, nick + " " + channelName);
        return;
    }

    // Store the invitation
    channel->invite(target);

    // Notify the inviter
    raiseReply(client->fd, RPL_INVITING, nick + " " + channelName);

    // Notify the client
    target->sendMessage(":" + client->getPrefix() + " INVITE " + nick + " :" + channelName);
}
