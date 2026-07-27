#include "../../includes/Server.hpp"

void    Server::nickCommand(Client *client, const Message &msg)
{
    if (!client->hasPassword)
    {
        raiseError(client->fd, ERR_NOTREGISTERED, "");
        return;
    }
    if (msg.params.size() < 1)
    {
        raiseError(client->fd, ERR_NONICKNAMEGIVEN, msg.command);
        return ;
    }
    /*to be added checking invalid nicknames*/

    /* unique nickname */
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->nickname == msg.params[0] && it->second != client)
        {
            raiseError(client->fd, ERR_NICKNAMEINUSE, msg.params[0]);
            return;
        }
    }
    client->nickname = msg.params[0];
    client->hasNick = true;
    tryAuthenticate(client->fd);
}
