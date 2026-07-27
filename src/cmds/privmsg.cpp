#include "../../includes/Server.hpp"

void Server::privmsgCommand(Client *client, const Message &msg)
{
    if (!client->authenticated)
    {
        raiseError(client->fd, ERR_NOTREGISTERED, "");
        return;
    }

    if (msg.params.empty())
    {
        raiseError(client->fd, ERR_NORECIPIENT, msg.command);
        return;
    }

    if (msg.trailing.empty())
    {
        raiseError(client->fd, ERR_NOTEXTTOSEND, "");
        return;
    }

    std::string target = msg.params[0];

    Client *receiver = getClientByNick(target);

    if (!receiver)
    {
        raiseError(client->fd, ERR_NOSUCHNICK, target);
        return;
    }

    receiver->sendMessage(
        ":" + client->getPrefix() +
        " PRIVMSG " +
        target +
        " :" +
        msg.trailing
    );
}