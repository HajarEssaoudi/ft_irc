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
#include "Client.hpp"
#include <cstdlib>
#include <signal.h>

class Channel;

class Server
{
    private:
        int _port;
        std::string _password;
        int _server_fd;
        std::vector<struct pollfd> _fds;
        std::map<int, Client*> _clients;
        std::vector<Channel*>_channels;
    public:
        static Server* instance;
        static void signalHandler(int signal);
        Server(int port , const std::string &password);
        ~Server();
        void start();
        const std::string& getPassword() const;
        std::map<int, Client*>& getClients();
        std::vector<Channel*>& getChannels();
        Client* getClientByNick(const std::string &nick);
        void removeClient(int fd);


    private:
        void setUpSocket();
        void accepterNewClient();
        void readClientData(int fd);
        void processMessage(int fd, const std::string& msg);
        void addPollFd(int fd);
        void removePollFd(int fd);
};
#endif