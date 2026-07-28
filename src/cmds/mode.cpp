#include "../../includes/Server.hpp"

void Server::modeCommand(Client *client, const Message &msg)
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
            raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
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
            raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
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
            raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
            return;
        }

        Client *target = requireClient(client, msg.params[2]);
        if (!target)
            return;

        if (!channel->hasMember(target))
        {
            raiseError(client->fd, ERR_USERNOTINCHANNEL, target->nickname + " " + channelName);
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
            raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
            return;
        }

        Client *target = requireClient(client, msg.params[2]);
        if (!target)
            return;

        if (!channel->hasMember(target))
        {
            raiseError(client->fd, ERR_USERNOTINCHANNEL, target->nickname + " " + channelName);
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

    // Uknown mode
    raiseError(client->fd, ERR_UNKNOWNMODE, mode);
}
