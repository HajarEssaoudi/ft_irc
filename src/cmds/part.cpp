#include "../../includes/Server.hpp"

void Server::partCommand(Client *client, const Message &msg)
{
    // Check parameters
    if (msg.params.empty())
    {
        client->sendMessage("461 PART :Not enough parameters\r\n");
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

    // Build the PART message
    std::string reply = ":" + client->getPrefix() +
                    " PART " + channelName;
    if (!msg.trailing.empty())
        reply += " :" + msg.trailing;

    reply += "\r\n";

    channel->broadcast(reply); // Broadcast

    channel->removeMember(client); // Remove the client

    // Remove operator status
    if (channel->isOperator(client))
        channel->removeOperator(client);
    
    // Remove the channel if it's empty
    if (channel->empty())
        removeChannel(channelName);
}
