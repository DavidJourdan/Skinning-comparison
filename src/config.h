#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    std::string inputFile;
    std::string skelFile;
    std::string weightFile;
    std::string outputFile;

    enum class Mode {
        precompute,
        gui,
    };

    Mode mode;

    Config(std::string inputFile,
           std::string skelFile,
           std::string weightFile,
           std::string outputFile,
           Mode mode) : inputFile { inputFile },
        skelFile { skelFile }, weightFile { weightFile },
        outputFile { outputFile }, mode { mode }
    {

    }
};

#endif // CONFIG_H
