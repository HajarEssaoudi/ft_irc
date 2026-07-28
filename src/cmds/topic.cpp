#include "../../includes/Server.hpp"

void Server::topicCommand(Client *client, const Message &msg)
{
    // Check parameters
    if (msg.params.empty())
    {
        raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
        return;
    }
    std::string channelName = msg.params[0];

    // Find the channel
    Channel *channel = requireChannel(client, channelName);
    if (!channel)
        return;

    // Check membership
    if (!requireMember(client, channel))
        return;

    // Reading topic case
    if (msg.trailing.empty())
    {
        if (channel->getTopic().empty())
        {
            raiseReply(client->fd, RPL_NOTOPIC, channelName);
        }
        else
        {
            raiseReply(client->fd, RPL_TOPIC, channelName, channel->getTopic());
        }
        return;
    }

    // Setting topic case, +t: only operators change the topic, -t: anyone
    if (channel->isTopicRestricted() && !requireOperator(client, channel))
        return;

    channel->setTopic(msg.trailing);

    std::string reply = ":" + client->getPrefix() + " TOPIC " + channelName + " :" + msg.trailing;
    channel->broadcast(reply);
}
