#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/utils.hpp"

void Server::tryAuthenticate(int fd)
{
    Client *client = _clients[fd];

    if (client->hasPassword &&
        client->hasNick &&
        client->hasUser)
    {
        /* to be fixed: do not display welcome msg in the other Nick*/
        client->authenticated = true;
        client->sendMessage(
            ":ircserv 001 " + client->nickname +
            " :Welcome to the Internet Relay Network " +
            client->getPrefix()
        );
    }
}

void Server::execCmd(Client *client , Message &msg)
{
    if (msg.command == "PASS")
        passCommand(client, msg);

    else if (msg.command == "NICK")
        nickCommand(client, msg);

    else if (msg.command == "USER")
        userCommand(client, msg);
    else if (msg.command == "JOIN")
        joinCommand(client, msg);

    else if (msg.command == "PRIVMSG")
        privmsgCommand(client, msg);

    else if (msg.command == "TOPIC")
        topicCommand(client, msg);

    else if (msg.command == "MODE")
        modeCommand(client, msg);

    else if (msg.command == "KICK")
        kickCommand(client, msg);

    else if (msg.command == "INVITE")
        inviteCommand(client, msg);
    else if (msg.command == "PART")
        partCommand(client, msg);
    else if (msg.command == "QUIT")
        quitCommand(client, msg);
    else if (msg.command == "CAP")
        return;
}
