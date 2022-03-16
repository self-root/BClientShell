#include "client.hpp"

#include <iostream>

int main()
{
    try
    {
        boost::asio::io_context context;
        Client client(context);

        client.startConnection("127.0.0.1", 9003);

        context.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}