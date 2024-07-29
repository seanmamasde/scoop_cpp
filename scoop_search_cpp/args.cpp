#include "args.hpp"

const std::string posh_hook =
"function scoop { if ($args[0] -eq \"search\") { scoop_search_cpp.exe "
"@($args | Select-Object -Skip 1) } else { scoop.ps1 @args } }";

bool args::parse(const int argc, char* argv[])
{
    if (argc == 2 && std::string(argv[1]) == "--hook")
    {
        // Invoke-Expression hook
        hook = true;
        return true;
    }
    if (argc == 3 && std::string(argv[1]) == "search")
    {
        // scoop search command
        query = argv[2];
        return true;
    }
    if (argc == 1)
    {
        // pass on to scoop
        return true;
    }
    return false;
}
