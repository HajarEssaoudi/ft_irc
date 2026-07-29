#include "../../includes/Server.hpp"

void Server::quitCommand(Client *client, const Message &msg)
{
    std::string reason = msg.trailing;

    if (reason.empty())
        reason = "Client Quit";

    std::string reply = ":" + client->getPrefix() + " QUIT :" + reason;

    std::map<std::string, Channel *>::iterator it = _channels.begin();

    while (it != _channels.end())
    {
        Channel *channel = it->second;

        if (channel->hasMember(client))
        {
            // Tell everyone in the channel that this client quit
            channel->broadcast(reply);

            // Remove the client
            channel->removeMember(client);

            // Delete the channel if it became empty
            if (channel->empty())
            {
                std::string name = channel->getName();

                ++it;
                removeChannel(name);
                continue;
            }
        }

        ++it;
    }
    //zedtha 
    client->sendMessage("ERROR :Closing connection :" + reason);
    removeClient(client->getFd());
}
