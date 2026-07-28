#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <poll.h>
#include <vector>
#include <map>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <cstdlib>
#include <signal.h>
#include <sstream> 
#include "Client.hpp"
#include "Channel.hpp"
#include "utils.hpp"

class Channel;

class Server
{
    private:
        int                         _port;
        std::string                 _password;
        int                         _server_fd;
        std::vector<struct pollfd>  _fds;
        std::map<int, Client*>      _clients;
        std::map<std::string, Channel*> _channels;

    public:
        static Server* instance;
        static void signalHandler(int signal);

        Server(int port, const std::string &password);
        ~Server();

        void start();

        const std::string&              getPassword() const;
        std::map<int, Client*>&         getClients();
        std::map<std::string, Channel*>& getChannels();
        Client*                         getClientByNick(const std::string &nick);
        void                            removeClient(int fd);

        
        Channel* getChannel(const std::string &name);
        Channel* createChannel(const std::string &name);
        void     removeChannel(const std::string &name);

        bool isValidCmd(Message& msg);
        void passCommand(Client *client, const Message &msg);
        void nickCommand(Client *client, const Message &msg);
        void userCommand(Client *client, const Message &msg);
        void joinCommand(Client *client, const Message &msg);
        void partCommand(Client *client, const Message &msg);
        void topicCommand(Client *client, const Message &msg);
        void inviteCommand(Client *client, const Message &msg);
        void kickCommand(Client *client, const Message &msg);
        void modeCommand(Client *client, const Message &msg);
        void privmsgCommand(Client *client, const Message &msg);
        void quitCommand(Client *client, const Message &msg);

        void tryAuthenticate(int fd);
        void raiseError(int fd, int code, const std::string &arg);
        void raiseReply(int fd, int code, const std::string &arg1, const std::string &arg2 = "");
        void execCmd(Client *client , Message &msg);

    private:
        void setUpSocket();
        void accepterNewClient();
        void readClientData(int fd);
        void processMessage(int fd, const std::string &msg);
        void addPollFd(int fd);
        void removePollFd(int fd);

        Channel* requireChannel(Client *client, const std::string &name);
        bool     requireMember(Client *client, Channel *channel);
        bool     requireOperator(Client *client, Channel *channel);
        Client*  requireClient(Client *requester, const std::string &nickname);
};

#endif