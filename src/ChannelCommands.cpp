#include "../includes/Server.hpp"

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

///// CHANNEL COMMANDS /////
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

void Server::kickCommand(Client *client, const Message &msg)
{
    // Check parameters
    if (msg.params.size() < 2)
    {
        client->sendMessage("461 KICK :Not enough parameters\r\n");
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
        client->sendMessage("441 " + nick +
                            " " + channelName +
                            " :They aren't on that channel\r\n");
        return;
    }

    // Build the kick message
    std::string reply = ":" + client->getPrefix() +
                    " KICK " +
                    channelName +
                    " " +
                    nick;
    if (!msg.trailing.empty())
        reply += " :" + msg.trailing;

    reply += "\r\n";
    channel->broadcast(reply); // Broadcast it

    // Remove the user
    channel->removeMember(target);
    if (channel->isOperator(target))
        channel->removeOperator(target);
    
    // Delete channel if its empty
    if (channel->empty())
        removeChannel(channelName);
}

void Server::modeCommand(Client *client, const Message &msg)
{
    // Check parameters
     if (msg.params.empty())
    {
        client->sendMessage("461 MODE :Not enough parameters\r\n");
        return;
    }
    std::string channelName = msg.params[0];

    // Find the channel
    Channel *channel = requireChannel(client, channelName);
    if (!channel)
        return;

    // Read current modes
    if (msg.params.size() == 1)
    {
        std::string modes = "+";

        if (channel->isInviteOnly())
            modes += "i";

        if (channel->isTopicRestricted())
            modes += "t";

        if (channel->hasKey())
            modes += "k";

        if (channel->hasLimit())
            modes += "l";

        client->sendMessage(":ircserv 324 " +
                            client->nickname +
                            " " +
                            channelName +
                            " " +
                            modes +
                            "\r\n");
        return;
    }

    // Permission checks
    if (!requireMember(client, channel))
        return;

    if (!requireOperator(client, channel))
        return;

    // Getting the second parameter
    std::string mode = msg.params[1];

    // Handling +i
    if (mode == "+i")
    {
        channel->setInviteOnly(true);

        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " +i\r\n";

        channel->broadcast(reply);
        return;
    }

    // Handling -i
    if (mode == "-i")
    {
        channel->setInviteOnly(false);

        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " -i\r\n";

        channel->broadcast(reply);
        return;
    }

    // Handling +t
    if (mode == "+t")
    {
        channel->setTopicRestricted(true);

        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " +t\r\n";

        channel->broadcast(reply);
        return;
    }

    // Handling -t
    if (mode == "-t")
    {
        channel->setTopicRestricted(false);

        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " -t\r\n";

        channel->broadcast(reply);
        return;
    }

    // Handling +k
    if (mode == "+k")
    {
        if (msg.params.size() < 3)
        {
            client->sendMessage("461 MODE :Not enough parameters\r\n");
            return;
        }
        std::string key = msg.params[2];
        channel->setKey(key);
        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " +k\r\n";

        channel->broadcast(reply);
        return;
    }

    // Handling -k
    if (mode == "-k")
    {
        channel->removeKey();

        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " -k\r\n";

        channel->broadcast(reply);
        return;
    }

    // Handling +l
    if (mode == "+l")
    {
        if (msg.params.size() < 3)
        {
            client->sendMessage("461 MODE :Not enough parameters\r\n");
            return;
        }

        int limit = std::atoi(msg.params[2].c_str());

        if (limit <= 0)
        {
            client->sendMessage(":ircserv 696 " +
                                client->nickname +
                                " " +
                                channelName +
                                " :Invalid user limit\r\n");
            return;
        }

        channel->setUserLimit(static_cast<size_t>(limit));

        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " +l\r\n";

        channel->broadcast(reply);

        return;
    }

    // Handling -l
    if (mode == "-l")
    {
        channel->removeUserLimit();

        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " -l\r\n";

        channel->broadcast(reply);

        return;
    }

    // Handling +o
    if (mode == "+o")
    {
        if (msg.params.size() < 3)
        {
            client->sendMessage("461 MODE :Not enough parameters\r\n");
            return;
        }

        Client *target = requireClient(client, msg.params[2]);
        if (!target)
            return;

        if (!channel->hasMember(target))
        {
            client->sendMessage("441 " +
                                target->nickname +
                                " " +
                                channelName +
                                " :They aren't on that channel\r\n");
            return;
        }

        channel->addOperator(target);

        std::string reply = ":" + client->getPrefix() +
                        " MODE " +
                        channelName +
                        " +o " +
                        target->nickname +
                        "\r\n";

        channel->broadcast(reply);

        return;
    }

    // Handling -o
    if (mode == "-o")
    {
        if (msg.params.size() < 3)
        {
            client->sendMessage("461 MODE :Not enough parameters\r\n");
            return;
        }

        Client *target = requireClient(client, msg.params[2]);
        if (!target)
            return;

        if (!channel->hasMember(target))
        {
            client->sendMessage("441 " +
                                target->nickname +
                                " " +
                                channelName +
                                " :They aren't on that channel\r\n");
            return;
        }

        channel->removeOperator(target);

        std::string reply = ":" + client->getPrefix() +
                            " MODE " +
                            channelName +
                            " -o " +
                            target->nickname +
                            "\r\n";

        channel->broadcast(reply);

        return;
    }

    // Uknown cases
    client->sendMessage(":ircserv 472 " +
                    mode +
                    " :is unknown mode char to me\r\n");
}
