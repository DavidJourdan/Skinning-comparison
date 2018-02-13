#ifndef CONFIG_H
#define CONFIG_H

#include <string>

using std::string;

struct Config {
    string inputFile;

    Config(string input) : inputFile { input }
    {

    }
};

#endif // CONFIG_H
