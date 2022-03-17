#include "client.hpp"

#include <iostream>

#ifdef _WIN32
#include "winuser.h"
#endif

int main()
{
    #ifdef _WIN32
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    #endif
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