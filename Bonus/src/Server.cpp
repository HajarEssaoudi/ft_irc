#include "../includes/Server.hpp"

Server *Server::instance = NULL;

Server::Server(int port , const std::string &password): _port(port), _password(password), _server_fd(-1){}


Server::~Server()
{
    for(std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete it->second;
    _clients.clear(); 
    std::map<std::string, Channel*>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;
    _channels.clear();
    if(_server_fd != -1)
        close(_server_fd);
    std::cout<< "Server stopped." <<std::endl;
}

//getters
std::map<int, Client*>& Server::getClients()
{
    return(_clients);
}

std::map<std::string, Channel*>& Server::getChannels()
{
    return(_channels);
}

const std::string& Server::getPassword()const
{
    return(_password);
}

Client* Server::getClientByNick(const std::string& nick)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->nickname == nick)
        {
            return it->second;
        }
    }
    return NULL;
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
        throw std::runtime_error("Error: socket() failed.");
    int opt = 1;
    if(setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("Error: setsockopt() failed.");

    int flags = fcntl(_server_fd, F_GETFL);
    if(fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("Error: fcntl() failed.");
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("Error: bind() failed. Port already in use?");
    if(listen(_server_fd, SOMAXCONN) == -1)
        throw std::runtime_error("Error: listen() failed.");

    addPollFd(_server_fd);
}
void Server::accepterNewClient()
{
    struct sockaddr_in clientAddr;

    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_server_fd, (struct sockaddr*)&clientAddr, &clientLen);
    if(clientFd == -1)
    {
        std::cerr << "Error: accept() failed." << std::endl;
        return;
    }
    int flags = fcntl(clientFd, F_GETFL);
    if(fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Error: fcntl() failed." << std::endl;
        close(clientFd);
        return;
    }
    _clients[clientFd] = new Client(clientFd);
    addPollFd(clientFd);
    std::cout << "New client connected (fd=" << clientFd << ")" << std::endl;
}

void Server::start()
{
    instance = this;
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    setUpSocket();
    std::cout << "Server launched on port " << _port << std::endl;
    std::cout << "Waiting for connections..." << std::endl;
    while(true)
    {
        int ready = poll(_fds.data(), _fds.size(), -1);
        if(ready == -1)
        {
            std::cerr << "Error: poll() failed." << std::endl;
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
            std::cout << "Client (fd=" << fd << ") disconnected." << std::endl;
        else
            std::cerr << "Error: recv() failed (fd=" << fd << ")." << std::endl;
        removeClient(fd);
        return;
    }
    if (_clients.find(fd) == _clients.end())
        return;
    _clients[fd]->buffer += std::string(buf, bytes);
    size_t pos;
    while((pos = _clients[fd]->buffer.find("\r\n")) != std::string::npos)
    {
        std::string msg = _clients[fd]->buffer.substr(0, pos);
        _clients[fd]->buffer.erase(0, pos + 2);
        if(!msg.empty())
            processMessage(fd, msg);
        if (_clients.find(fd) == _clients.end())
        return;
    }
}
void Server::processMessage(int fd, const std::string &msg)
{
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;

    Client *client = it->second;
    Message message = parseLine(msg);

    if (message.command.empty())
        return;
    execCmd(client , message);
}

void Server::removeClient(int fd)
{
    if(_clients.find(fd) == _clients.end())
        return;
    std::cout << "Client removed (fd=" << fd  << ", nick=" << _clients[fd]->nickname << ")" << std::endl;
    delete _clients[fd];
    _clients.erase(fd);
    removePollFd(fd);
}

Channel* Server::getChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);

    if (it == _channels.end())
        return (NULL);

    return (it->second);
}

Channel* Server::createChannel(const std::string &name)
{
    Channel *channel = getChannel(name);

    if (channel)
        return (channel);

    channel = new Channel(name);
    _channels[name] = channel;

    return (channel);
}

void Server::removeChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);

    if (it == _channels.end())
        return;

    delete it->second;
    _channels.erase(it);
}

void Server::signalHandler(int signal)
{
    std::cout << "\nSignal " << signal << " received. Shutting down server..." << std::endl;
    return;
}
