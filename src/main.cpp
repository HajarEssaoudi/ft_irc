#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include <cstdlib>
#include <stdexcept>
#include "../includes/parsing.hpp"

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    int port = atoi(argv[1]);

    if (port <= 1024 || port > 65535)
    {
        std::cerr << "Error: invalid port" << std::endl;
        return 1;
    }
    std::string password = argv[2];
    if (password.empty())
    {
        std::cerr << "Error empty pwd" << std::endl;
        return 1;
    }
    try
    {
        Server server(port, password);
        server.start();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error" << e.what() << std::endl;
        return 1;
    }

    return 0;
}
