#include "../../includes/Server.hpp"

void Server::pingCommand(Client *client, Message &msg)
{
    if (!msg.trailing.empty())
        client->sendMessage("PONG :" + msg.trailing);
    else if (!msg.params.empty())
        client->sendMessage("PONG :" + msg.params[0]);
    else
        raiseError(client->fd, ERR_NOORIGIN, "");
}
