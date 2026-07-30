#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/utils.hpp"

void Server::tryAuthenticate(int fd)
{
    Client *client = _clients[fd];

    if (client->authenticated)
        return;
    if (client->hasPassword &&
        client->hasNick &&
        client->hasUser)
    {
        client->authenticated = true;
        client->sendMessage(
            ":ircserv 001 " + client->nickname +
            " :Welcome to the Internet Relay Network " +
            client->getPrefix()
        );
             client->sendMessage(
            ":ircserv 002 " + client->nickname +
            " :Your host is ircserv"
        );

        client->sendMessage(
            ":ircserv 003 " + client->nickname +
            " :This server was created today"
        );

        client->sendMessage(
            ":ircserv 004 " + client->nickname +
            " ircserv 1.0"
        );
    }
}



void Server::execCmd(Client *client, Message &msg)
{
    if (msg.command == "PASS") passCommand(client, msg);

    else if (msg.command == "NICK") nickCommand(client, msg);

    else if (msg.command == "USER") userCommand(client, msg);

    else if (msg.command == "JOIN") joinCommand(client, msg);

    else if (msg.command == "PRIVMSG") privmsgCommand(client, msg);

    else if (msg.command == "TOPIC") topicCommand(client, msg);

    else if (msg.command == "MODE") modeCommand(client, msg);

    else if (msg.command == "KICK") kickCommand(client, msg);

    else if (msg.command == "INVITE") inviteCommand(client, msg);

    else if (msg.command == "PART") partCommand(client, msg);
    else if (msg.command == "QUIT") quitCommand(client, msg);

    else if (msg.command == "PING") pingCommand(client, msg);
    else if (msg.command == "WHOIS") whoisCommand(client, msg);
    else if (msg.command == "CAP")
    {
        client->sendMessage(":ircserv CAP * LS :");
        return;
    }
    else
        raiseError(client->fd, ERR_UNKNOWNCOMMAND, msg.command);
}