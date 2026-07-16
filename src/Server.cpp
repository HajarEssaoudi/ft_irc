#include "../includes/Server.hpp"

Server::Server(int port , const std::string &password): _port(port), _password(password), _server_fd(-1){}


Server::~Server()
{
    for(std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete it->second;
    _channels.clear();

    if(_server_fd != -1)
        close(_server_fd);
    std::cout<<"Server stopped"<<std::endl;
}

//getters
std::map<int, Client*>& Server::getClients()
{
    return(_clients);
}

std::vector<Channel *>& Server::getChannels()
{
    return(_channels);
}

const std::string& Server::getPassword()const
{
    return(_password);
}

Client* Server::getClientByNick(const std::string& nick)
{
    for(std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if(it->second->nickname == nick)
            return(it->second);
    }
    return(NULL);
}

void Server::addPollFd(int fd)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);
}

void Server::removePollFd(int fd)
{
    for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
    {
        if (it->fd == fd)
        {
            _fds.erase(it);
            return;
        }
    }
}

void Server::setUpSocket()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_server_fd == -1)
        throw std::runtime_error("Socket()_ERROR");
    int opt = 1;
    if(setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("setsockopt()_ERROR");

    // fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK);
    int flags = fcntl(_server_fd, F_GETFL);

    if(fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("fctl_ERROR");
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("bind()_ERROR ---- Port already used?");
    if(listen(_server_fd, SOMAXCONN) == -1)
        throw std::runtime_error("listen()_ERROR");

    addPollFd(_server_fd);
}
void Server::accepterNewClient()
{
    struct sockaddr_in clientAddr;

    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_server_fd, (struct sockaddr*)&clientAddr, &clientLen);
    if(clientFd == -1)
    {
        std::cerr<<"Accept()_ERROR\n";
        return;
    }  
    if(fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr<<"fcntl()_ERROR\n";
        close(clientFd);
        return;
    }
    _clients[clientFd] = new Client(clientFd);
    addPollFd(clientFd);
    std::cout<<"Client connected fd = "<<clientFd <<std::endl;
}

void Server::start()
{
    setUpSocket();
    std::cout<<"the server has been launched"<<_port <<std::endl;
    std::cout<<"waiting..."<<std::endl;
    while(true)
    {
        int ready = poll(_fds.data(), _fds.size(), -1);
        if(ready == -1)
        {
            std::cerr<<"poll_ERROR"<<std::endl;
            break;
        }
        size_t size = _fds.size();
        for(size_t i = 0; i < size; i++)
        {
            if(_fds[i].revents & POLLIN)
            {
                if(_fds[i].fd == _server_fd)
                    accepterNewClient();
                else
                    readClientData(_fds[i].fd);
            }
        }
    }

}

void Server::readClientData(int fd)
{
    char buf[512];
    memset(buf, 0, sizeof(buf));
    int bytes = recv(fd, buf, sizeof(buf) -1, 0);
    if(bytes <=0)
    {
        if(bytes == 0)
            std::cout<<"Client fd= "<<fd<<"disconnected"<<std::endl;
        else
            std::cerr<<"recv()_ERROR fd= "<<fd<<std::endl;
        removeClient(fd);
        return;
    }
    _clients[fd]->buffer += std::string(buf, bytes);
    size_t pos;
    while((pos = _clients[fd]->buffer.find("\r\n")) != std::string::npos)
    {
        std::string msg = _clients[fd]->buffer.substr(0, pos);
        _clients[fd]->buffer.erase(0, pos + 2);
        if(!msg.empty())
            processMessage(fd, msg);
    }
}
void Server:: processMessage(int fd, const std::string& msg)
{
    std::cout << "msg received fd=" << fd << " : [" << msg << "]" << std::endl;

    //parsing

    if (_clients.find(fd) != _clients.end())
        _clients[fd]->sendMessage(":server notice * :Msg reçu : " + msg);
}
void Server::removeClient(int fd)
{
    if(_clients.find(fd) == _clients.end())
        return;
    std::cout<<"Supp client fd= "<<fd<<"nickname: "<<_clients[fd]->nickname<<std::endl;
    delete _clients[fd];
    _clients.erase(fd);
    removePollFd(fd);
}