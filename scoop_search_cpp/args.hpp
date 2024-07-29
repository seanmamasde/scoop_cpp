#pragma once

#include <string>

extern const std::string posh_hook;

class args
{
public:
    bool hook{ false };
    std::string query{};

    bool parse(int argc, char* argv[]);
};
