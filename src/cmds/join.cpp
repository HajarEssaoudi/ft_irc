#include "../../includes/Server.hpp"

void Server::joinCommand(Client *client, const Message &msg)
{
    // Validate parameters
    if (msg.params.empty())
    {
        client->sendMessage("461 JOIN :Not enough parameters\r\n");
        return;
    }
    std::string channelName = msg.params[0];

    // Validate the channel name
    if (channelName.empty() || channelName[0] != '#')
    {
        client->sendMessage("403 " + channelName + " :No such channel\r\n");
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
        client->sendMessage("473 " + channelName +
                            " :Cannot join channel (+i)\r\n");
        return;
    }

    // Check key
    if (channel->hasKey())
    {
        if (msg.params.size() < 2 || !channel->checkKey(msg.params[1]))
        {
            client->sendMessage("475 " + channelName +
                                " :Cannot join channel (+k)\r\n");
            return;
        }
    }

    // Check the user limit
    if (channel->isFull())
    {
        client->sendMessage("471 " + channelName +
                            " :Cannot join channel (+l)\r\n");
        return;
    }

    channel->addMember(client); // add member
    // If first member become the operator
    if (channel->getMemberCount() == 1)
        channel->addOperator(client);
    
    // If the user joined using an invitation, remove it
    if (channel->isInvited(client))
        channel->removeInvite(client);
    
    // Broadcast the join
    std::string reply = ":" + client->getPrefix() +
                    " JOIN " + channelName + "\r\n";
    channel->broadcast(reply);

    // Topic
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
            " :" + channel->getTopic() + "\r\n");
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
    client->sendMessage(":ircserv 353 " +
    client->nickname +
    " = " +
    channelName +
    " :" +
    names +
    "\r\n");
    client->sendMessage(":ircserv 366 " +
    client->nickname +
    " " +
    channelName +
    " :End of /NAMES list\r\n");
}
