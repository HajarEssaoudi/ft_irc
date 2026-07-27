#include "../../includes/Server.hpp"

void    Server::userCommand(Client *client, const Message &msg)
{
    if (!client->hasPassword)
    {
        raiseError(client->fd, ERR_NOTREGISTERED, "");
        return;
    }
    if (client->hasUser)
    {
        raiseError(client->fd, ERR_ALREADYREGISTERED, msg.command);
        return;
    }
    if (msg.trailing.empty())
    {
        raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
        return;
    }
    client->username = msg.params[0];
    client->realname = msg.trailing;
    client->hasUser = true;
    tryAuthenticate(client->fd);
}
