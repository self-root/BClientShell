#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#ifdef __linux__
#include <unistd.h>
#elif __WIN32
#include <direct.h>
#endif
#include <exception>
#include <string>
#include <string_view>
#include <cstdio>
#include <fstream>
#include <memory>
#include <vector>
#include <tuple>


namespace util {

class DirNotFound : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Directory not found\n";
    }
};

class ReadFileException : std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Error while opening the file\n";
    }
};

class Utility
{
public:
    static std::string currentDir()
    {
        char buffer[FILENAME_MAX];
        auto currDir = getcwd(buffer, FILENAME_MAX);

        return std::string(currDir);
    }

    static void setCurrendDir(std::string_view dirName)
    {
        std::cout << "Changing dir to: " << dirName << std::endl;
        int res = chdir(dirName.data());

        if (res != 0)
            throw DirNotFound();
    }

    static std::string excuteCommand(std::string_view command)
    {
        const int BUFFER_SIZE = 256;
        char buffer[BUFFER_SIZE];
        std::string output = "";
        FILE *pipe = popen(command.data(), "r");

        if (!pipe)
            return "Unknown error";
        while (!feof(pipe))
        {
            if (fgets(buffer, BUFFER_SIZE, pipe))
                output += buffer;
        }

        pclose(pipe);

        return output;
    }

    static std::vector<std::string> splitString(const std::string &string_, const char delimiter_)
    {
        std::vector<std::string> splitted;
        std::string buffer = "";
        for (auto c : string_)
        {
            if (c == delimiter_)
            {
                splitted.push_back(buffer);
                buffer = "";
            }

            else
            {
                buffer += c;
            }
        }

        if (!buffer.empty())
            splitted.push_back(buffer);

        return splitted;
    }

    static std::string joinString(const std::vector<std::string> &stringVector, int beg = 0)
    {
        std::string newStr = "";

        for (int i = beg; i < stringVector.size(); i++)
            newStr += stringVector.at(i);

        return newStr;
    }
};

class File
{
private:
    std::string mFileName;
    //std::streampos mFileSize;

    std::streampos fileSize(std::ifstream &fileStream)
    {
        fileStream.seekg(0, std::ios::end);
        auto fileSize_ = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);

        return fileSize_;
    }
public:
    File(std::string fileName)
    : mFileName(fileName)
    {
    }

    const char* read()
    {
        std::ifstream fileStream(mFileName, std::ios::binary);
        auto fileSize_ = fileSize(fileStream);
        std::cout << "Reading file";
        char *container = new char[fileSize_ + std::streamsize(1)];
        fileStream.read(container, fileSize_);
        fileStream.close();

        return container;
    }

    std::vector<uint8_t> readBinary()
    {
        std::ifstream file(mFileName, std::ios::binary);
        file.unsetf(std::ios::skipws);
        auto fileSize_ = fileSize(file);

        std::vector<uint8_t> data;
        data.reserve(fileSize_);
        data.insert(
            data.begin(),
            std::istream_iterator<uint8_t>(file),
            std::istream_iterator<uint8_t>()
            );

        return data;
    }

    ~File()
    {
    }
};

}

#endif // UTILITY_H
