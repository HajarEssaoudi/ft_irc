#include "../../includes/Server.hpp"

void Server::inviteCommand(Client *client, const Message &msg)
{
    // Check parameters
    if (msg.params.size() < 2)
    {
        client->sendMessage("461 INVITE :Not enough parameters\r\n");
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

    // Targeet already in channel?
    if (channel->hasMember(target))
    {
        client->sendMessage("443 " + nick +
                            " " + channelName +
                            " :is already on channel\r\n");
        return;
    }

    // Store the invitation
    channel->invite(target); // The invitation allows the client to JOIN later

    // Notify the inviter
    client->sendMessage(":ircserv 341 " +
                    client->nickname +
                    " " +
                    nick +
                    " " +
                    channelName +
                    "\r\n");
    // Notify the client
    target->sendMessage(":" +
                    client->getPrefix() +
                    " INVITE " +
                    nick +
                    " :" +
                    channelName +
                    "\r\n");
}
