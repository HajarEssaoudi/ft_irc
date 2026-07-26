#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/parsing.hpp"

void Server::tryAuthenticate(int fd)
{
    Client *client = _clients[fd];

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
    }
}

void    Server::executePass(Message msg, int fd)
{
    if (this->_clients[fd]->isAuthenticated())
    {
        raiseError(fd, ERR_ALREADYREGISTERED, msg.command);
        return;
    }
    if (msg.params.size() < 1)
    {
        raiseError(fd, ERR_NEEDMOREPARAMS, msg.command);
        return;
    }
    if (msg.params[0] != this->_password)
    {
        raiseError(fd, ERR_PASSWDMISMATCH, msg.command);
        return ;
    }
    this->_clients[fd]->hasPassword = true;
    tryAuthenticate(fd);
}

void    Server::executeUser(Message msg, int fd)
{
    if (this->_clients[fd]->hasUser)
    {
        raiseError(fd, ERR_ALREADYREGISTERED, msg.command);
        return;
    }
    if (msg.trailing.empty())
    {
        raiseError(fd, ERR_NEEDMOREPARAMS, msg.command);
        return;
    }
    this->_clients[fd]->username = msg.params[0];
    this->_clients[fd]->realname = msg.trailing;
    this->_clients[fd]->hasUser = true;
    tryAuthenticate(fd);
}

void    Server::executeNick(Message msg, int fd)
{
    if (msg.params.size() < 1)
    {
        raiseError(fd, ERR_NONICKNAMEGIVEN, msg.command);
        return ;
    }
    /*to be added checking invalid nicknames*/

    /* unique nickname */
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->nickname == msg.params[0] && it->first != fd)
        {
            raiseError(fd, ERR_NICKNAMEINUSE, msg.params[0]);
            return;
        }
    }
    this->_clients[fd]->nickname = msg.params[0];
    this->_clients[fd]->hasNick = true;
    tryAuthenticate(fd);
}

void Server::execCmd(const Message &msg, int fd)
{
    if (msg.command == "PASS")
        executePass(msg, fd);

    else if (msg.command == "NICK")
        executeNick(msg, fd);

    else if (msg.command == "USER")
        executeUser(msg, fd);
    // else if (msg.command == "JOIN")
    //     executeJoin(msg, fd);

    // else if (msg.command == "PRIVMSG")
    //     executePrivmsg(msg, fd);

    // else if (msg.command == "TOPIC")
    //     executeTopic(msg, fd);

    // else if (msg.command == "MODE")
    //     executeMode(msg, fd);

    // else if (msg.command == "KICK")
    //     executeKick(msg, fd);

    // else if (msg.command == "INVITE")
    //     executeInvite(msg, fd);
}
