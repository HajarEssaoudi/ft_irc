#include "../includes/Client.hpp"

Client:: Client(int fd):
    fd(fd),
    nickname("*"),
    username(""),
    realname(""),
    buffer(""),
    hasPassword(false),
    hasNick(false),
    hasUser(false),
    authenticated(false)
{}

Client:: ~Client()
{
    close(fd);
}
int Client::getFd()const
{
    return(fd);
}

bool Client::isAuthenticated() const
{
    return(authenticated);

}

std::string Client::getPrefix()const
{
    return(nickname + "!" + username + "@localhost");
}

void Client::sendMessage(const std::string& msg)
{
    std::string full = msg + "\r\n";
    if(send(fd, full.c_str(), full.size(), 0) == -1)
        std::cerr <<"send()_ERROR fd= "<<fd<<std::endl;
}