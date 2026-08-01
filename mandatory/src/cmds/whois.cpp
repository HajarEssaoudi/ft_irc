#include "../../includes/Server.hpp"

void Server::whoisCommand(Client *client, const Message &msg)
{
    if (msg.params.empty())
    {
        raiseError(client->fd, ERR_NEEDMOREPARAMS, msg.command);
        return;
    }

    Client *target = getClientByNick(msg.params[0]);

    if (!target)
    {
        raiseError(client->fd, ERR_NOSUCHNICK, msg.params[0]);
        return;
    }

    raiseReply(client->fd,
               RPL_WHOISUSER,
               target->nickname,
               target->username + " localhost * :" + target->realname);

    raiseReply(client->fd,
               RPL_ENDOFWHOIS,
               target->nickname,
               "");
}