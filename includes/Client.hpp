#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h> 

class Client
{
    public:
        int fd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string buffer;
        // Client();
        Client(int fd);
        ~Client();
        //step d'auth
        bool hasPassword;
        bool hasNick;
        bool hasUser;
        bool authenticated;

        int getFd() const;
        void appendBuffer(const std::string &data);
        std::string &getBuffer();
        //
        void sendMessage(const std::string& msg);
        bool isAuthenticated() const;
        std::string getPrefix() const;

};
#endif