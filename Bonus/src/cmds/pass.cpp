#include "../../includes/Server.hpp"

void    Server::passCommand(Client *client, const Message &msg)
{
    if (client->isAuthenticated())
    {
        raiseError(client->fd, ERR_ALREADYREGISTERED, msg.command);
        return;
    }
    if (msg.params.size() < 1)
    {
        raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
        return;
    }
    if (msg.params[0] != this->_password)
    {
        raiseError(client->fd, ERR_PASSWDMISMATCH, msg.command);
        return ;
    }
    client->hasPassword = true;
    tryAuthenticate(client->fd);
}