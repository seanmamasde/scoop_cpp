#include <iostream>
#include <string>
#include <vector>

#include "color.hpp"
#include "constants.hpp"
#include "env.hpp"
#include "parser.hpp"

parser::parser(const int argc, char* argv[], env& env)
{
    for (int i = 0; i < argc; ++i)
    {
        argv_.emplace_back(argv[i]);
    }

    if (argc == 1)
    {
        std::cout << usage;
        return;
    }
    if (argc == 2 && std::string(argv[1]) == "--hook")
    {
        std::cout << hook;
        return;
    }
    if (argc > 2 && command_map_.contains(std::string(argv[1])))
    {
        // pop the "scoop" and the command
        argv_ = std::vector<std::string>(argv_.begin() + 2, argv_.end());

        const auto command = command_map_.at(argv[1])();
        command->execute(env);
    }
    else
    {
        std::cerr
            << dye::yellow("Unknown command: ")
            << dye::yellow(argv[1])
            << "\n"
            << usage
            << "\n";
    }
}