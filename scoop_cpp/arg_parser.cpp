#include <iostream>

#include "arg_parser.hpp"
#include "../search/search.hpp"

bool parse(const int argc, char* argv[], env &env)
{
    if (argc == 1)
    {
        // scoop
        std::cout << usage << "\n";
        return true;
    }
    if (argc == 2 && std::string(argv[1]) == "--hook")
    {
        // Invoke-Expression (& scoop_cpp --hook)
        std::cout << hook << "\n";
    }
    if (argc >= 2)
    {
        switch (command_map[std::string(argv[1])])
        {
        case command_type::search_:
            {
                // scoop search <query>
                if (argc == 2)
                    std::cerr << color_map[color::yellow] << "Missing query, listing all packages\n" << color_map[color::reset];
                search_command cmd{ (argc >= 2) ? std::vector<std::string>(argv + 2, argv + argc) : std::vector<std::string>{""} };
                cmd.execute(env);
            }
            break;
        default:
            std::cerr << color_map[color::red] << "Not implemented yet :(\n" << color_map[color::reset];
            return false;
        }
        return true;
    }

    // cannot parse this command
    return false;
}
