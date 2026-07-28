#include "../../includes/Server.hpp"

void Server::joinCommand(Client *client, const Message &msg)
{
    // Validate parameters
    if (msg.params.empty())
    {
        raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
        return;
    }

    std::string channelName = msg.params[0];

    // Validate the channel name
    if (channelName.empty() || channelName[0] != '#')
    {
        raiseError(client->fd, ERR_NOSUCHCHANNEL, channelName);
        return;
    }

    // Find or create the channel
    Channel *channel = getChannel(channelName);
    if (!channel)
        channel = createChannel(channelName);

    // Prevent duplicate joins
    if (channel->hasMember(client))
        return;

    // Check invite-only
    if (channel->isInviteOnly() && !channel->isInvited(client))
    {
        raiseError(client->fd, ERR_INVITEONLYCHAN, channelName);
        return;
    }

    // Check key
    if (channel->hasKey())
    {
        if (msg.params.size() < 2 || !channel->checkKey(msg.params[1]))
        {
            raiseError(client->fd, ERR_BADCHANNELKEY, channelName);
            return;
        }
    }

    // Check the user limit
    if (channel->isFull())
    {
        raiseError(client->fd, ERR_CHANNELISFULL, channelName);
        return;
    }

    // Add member
    channel->addMember(client);

    // First member becomes operator
    if (channel->getMemberCount() == 1)
        channel->addOperator(client);

    // Remove invitation if it exists
    if (channel->isInvited(client))
        channel->removeInvite(client);

    // Broadcast the join
    std::string reply = ":" + client->getPrefix() + " JOIN " + channelName;
    channel->broadcast(reply);

    // Topic
    if (channel->getTopic().empty())
    {
        raiseReply(client->fd, RPL_NOTOPIC, channelName);
    }
    else
    {
        raiseReply(client->fd, RPL_TOPIC, channelName, channel->getTopic());
    }

    // Members list
    std::string names;
    std::set<Client*>::const_iterator it;

    for (it = channel->getMembers().begin();
         it != channel->getMembers().end();
         ++it)
    {
        if (!names.empty())
            names += " ";

        if (channel->isOperator(*it))
            names += "@";

        names += (*it)->nickname;
    }

    raiseReply(client->fd, RPL_NAMREPLY, channelName, names);

    raiseReply(client->fd, RPL_ENDOFNAMES, channelName);
}
