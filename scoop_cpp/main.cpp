#include <iostream>
#include <iosfwd>

#include "env.hpp"
#include "parser.hpp"

int main(const int argc, char **argv)
{
    // speedup blackmagic
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    // configure env and parse input arguments
    env env;
    parser parser(argc, argv, env);

    return 0;
}