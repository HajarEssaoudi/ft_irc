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
#include "parsing.hpp"

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
        Server(int port , const std::string &password);
        ~Server();
        void start();
        const std::string& getPassword() const;
        std::map<int, Client*>& getClients();
        std::vector<Channel*>& getChannels();
        Client* getClientByNick(const std::string &nick);
        void removeClient(int fd);
        void raiseError(int fd, int code, const std::string &arg); //added
        
        
        
        public: //to be changed after tests
        void setUpSocket();
        void accepterNewClient();
        void readClientData(int fd);
        void processMessage(int fd, const std::string& msg);
        void addPollFd(int fd);
        void removePollFd(int fd);
        void tryAuthenticate(int fd);
        /*cmds*/
        void    executePass(Message msg, int fd);
        void    executeNick(Message msg, int fd);
        void    executeUser(Message msg, int fd);

        void    execCmd(const Message &msg, int fd);
};
#endif