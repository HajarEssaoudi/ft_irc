#include "../../includes/Server.hpp"

void Server::topicCommand(Client *client, const Message &msg)
{
    // Check parameters
    if (msg.params.empty())
    {
        client->sendMessage("461 TOPIC :Not enough parameters\r\n");
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
            client->sendMessage(":ircserv 331 " +
                                client->nickname + " " +
                                channelName +
                                " :No topic is set\r\n");
        }
        else
        {
            client->sendMessage(":ircserv 332 " +
                                client->nickname + " " +
                                channelName +
                                " :" + channel->getTopic() +
                                "\r\n");
        }
        return;
    }

    // Setting topic case, +t: only operators change the topic, -t: anyone
    if (channel->isTopicRestricted() && !requireOperator(client, channel))
        return;

    channel->setTopic(msg.trailing);

    std::string reply = ":" + client->getPrefix() +
                        " TOPIC " +
                        channelName +
                        " :" +
                        msg.trailing +
                        "\r\n";
    channel->broadcast(reply);
}
