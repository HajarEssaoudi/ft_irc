#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/utils.hpp"

void Server::tryAuthenticate(int fd)
{
    Client *client = _clients[fd];

    if (client->hasPassword &&
        client->hasNick &&
        client->hasUser)
    {
        client->authenticated = true;
        client->sendMessage(
            ":ircserv 001" + client->nickname +
            " :Welcome to the Internet Relay Network " +
            client->getPrefix()
        );
    }
}

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

    // else if (msg.command == "PRIVMSG")
    //     privmsgCommand(client, msg);

    else if (msg.command == "TOPIC")
        topicCommand(client, msg);

    else if (msg.command == "MODE")
        modeCommand(client, msg);

    else if (msg.command == "KICK")
        kickCommand(client, msg);

    else if (msg.command == "INVITE")
        inviteCommand(client, msg);
}
