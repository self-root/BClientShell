#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <nlohmann/json.hpp>

#include "utils/utility.h"

using json = nlohmann::json;
using byte = unsigned char;
const unsigned int BUFFER_SIZE = 1024;

class Client : public std::enable_shared_from_this<Client>
{
private:
    boost::asio::ip::tcp::socket socket;
    boost::asio::io_context &context;
    boost::asio::ip::tcp::endpoint endpoint;
    std::array<byte, BUFFER_SIZE> buffer;
    std::vector<uint8_t> data_;

public:
    Client(boost::asio::io_context &context_);
    ~Client();

    /**
     * @brief Start connection to the server
     * 
     * @param hostIp A V4 Ip address 
     * @param port 
     */
    void startConnection(std::string_view hostIp, unsigned short port);
private:
    /**
     * @brief Start receiving data asynchronously from the server
     * 
     */
    void receiveData();

    /**
     * @brief Handle the reading of new data
     * 
     * @param er error code if any
     * @param data 
     */
    void handleReading(const boost::system::error_code &er, std::size_t data);

    /**
     * @brief parse the binary data into json object to retrieve information
     * 
     * @param binaryData_ 
     */
    void handleBinData(const std::vector<uint8_t> &binaryData_);

    /**
     * @brief Send response back to the server
     * 
     * @param response 
     */
    void sendResponse(std::string_view response);
    /**
     * @brief Send response back to the server along the file
     * 
     * @param response 
     * @param data 
     */
    void sendResponse(std::string_view response, const std::vector<uint8_t> &data);

    void send(const std::vector<uint8_t> &dataToSend);

    /**
     * @brief Prepare file requested by the server
     * 
     * @param filename 
     */
    void prepareFile(std::string_view filename);

    /**
     * @brief execu
     * 
     * @param program executable name
     * @param args arguments
     * @return the output of the executed command
     */
    std::string runCommand(std::string_view program, std::string_view args);
};

#endif // CLIENT_HPP