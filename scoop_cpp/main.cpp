#include <iostream>

#include "arg_parser.hpp"

int main(const int argc, char* argv[])
{
    // speedup blackmagic
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    // configure env and parse input arguments
    if (env env{}; !parse(argc, argv, env))
        return 1;

    return 0;
}