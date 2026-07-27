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

// void Server::execCmd(Client *client , Message &msg)
// {
//     if (msg.command == "PASS")
//         passCommand(client, msg);

//     else if (msg.command == "NICK")
//         nickCommand(client, msg);

//     else if (msg.command == "USER")
//         userCommand(client, msg);
//     else if (msg.command == "JOIN")
//         joinCommand(client, msg);

//     else if (msg.command == "PRIVMSG")
//         privmsgCommand(client, msg);

//     else if (msg.command == "TOPIC")
//         topicCommand(client, msg);

//     else if (msg.command == "MODE")
//         modeCommand(client, msg);

//     else if (msg.command == "KICK")
//         kickCommand(client, msg);

//     else if (msg.command == "INVITE")
//         inviteCommand(client, msg);
//     else if (msg.command == "PART")
//         partCommand(client, msg);
//     else if (msg.command == "CAP")
//         return;
// }


void Server::execCmd(Client *client, Message &msg)
{
    std::cout << "\n========== NEW COMMAND ==========\n";
    std::cout << "Command = [" << msg.command << "]" << std::endl;

    for (size_t i = 0; i < msg.params.size(); i++)
        std::cout << "Param " << i << " = [" << msg.params[i] << "]" << std::endl;

    std::cout << "Trailing = [" << msg.trailing << "]" << std::endl;
    std::cout << "=================================\n";

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

    else if (msg.command == "CAP")
    {
        client->sendMessage(":ircserv CAP * LS :\r\n");
        return;
    }
}