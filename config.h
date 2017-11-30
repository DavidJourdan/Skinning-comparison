#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    std::string inputFile;
    std::string skelFile;
    std::string weightFile;

    Config(std::string inputFile,
           std::string skelFile,
           std::string weightFile) : inputFile { inputFile },
        skelFile { skelFile }, weightFile { weightFile }
    {

    }
};

#endif // CONFIG_H
