#pragma once

#include <string>
#include <unordered_map>

#include "env.hpp"

constexpr const char* hook = 1 + R"(
function scoop { 
    if ($args[0] -eq "search") { 
        scoop_search_cpp.exe @($args | Select-Object -Skip 1) 
    } else { 
        scoop.ps1 @args 
    } 
})";

constexpr const char* usage = 1 + R"(
Usage: scoop <command> [<args>]

Available commands are listed below.

Type 'scoop help <command>' to get more help for a specific command.
)"
"\033[32m" /*green*/ R"(
Command    Summary
-------    -------
)" "\033[0m" /*reset*/
R"(alias      Manage scoop aliases
bucket     Manage Scoop buckets
cache      Show or clear the download cache
cat        Show content of specified manifest.
checkup    Check for potential problems
cleanup    Cleanup apps by removing old versions
config     Get or set configuration values
create     Create a custom app manifest
depends    List dependencies for an app, in the order they'll be installed
download   Download apps in the cache folder and verify hashes
export     Exports installed apps, buckets (and optionally configs) in JSON format
help       Show help for a command
hold       Hold an app to disable updates
home       Opens the app homepage
import     Imports apps, buckets and configs from a Scoopfile in JSON format
info       Display information about an app
install    Install apps
list       List installed apps
prefix     Returns the path to the specified app
reset      Reset an app to resolve conflicts
search     Search available apps
shim       Manipulate Scoop shims
status     Show status and check for new app versions
unhold     Unhold an app to enable updates
uninstall  Uninstall an app
update     Update apps, or Scoop itself
virustotal Look for app's hash or url on virustotal.com
which      Locate a shim/executable (similar to 'which' on Linux))";

// added underscore to avoid conflict with C++ keyword
enum class command_type : std::uint8_t
{
    alias_,
    bucket_,
    cache_,
    cat_,
    checkup_,
    cleanup_,
    config_,
    create_,
    depends_,
    download_,
    export_,
    help_,
    hold_,
    home_,
    import_,
    info_,
    install_,
    list_,
    prefix_,
    reset_,
    search_,
    shim_,
    status_,
    unhold_,
    uninstall_,
    update_,
    virustotal_,
    which_
};

inline std::unordered_map<std::string, command_type> command_map = {
    {"alias", command_type::alias_},
    {"bucket", command_type::bucket_},
    {"cache", command_type::cache_},
    {"cat", command_type::cat_},
    {"checkup", command_type::checkup_},
    {"cleanup", command_type::cleanup_},
    {"config", command_type::config_},
    {"create", command_type::create_},
    {"depends", command_type::depends_},
    {"download", command_type::download_},
    {"export", command_type::export_},
    {"help", command_type::help_},
    {"hold", command_type::hold_},
    {"home", command_type::home_},
    {"import", command_type::import_},
    {"info", command_type::info_},
    {"install", command_type::install_},
    {"list", command_type::list_},
    {"prefix", command_type::prefix_},
    {"reset", command_type::reset_},
    {"search", command_type::search_},
    {"shim", command_type::shim_},
    {"status", command_type::status_},
    {"unhold", command_type::unhold_},
    {"uninstall", command_type::uninstall_},
    {"update", command_type::update_},
    {"virustotal", command_type::virustotal_},
    {"which", command_type::which_}
};

enum class color : std::uint8_t
{
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    reset
};

inline std::unordered_map<color, std::string> color_map = {
    {color::red, "\033[1;31m"},
    {color::green, "\033[1;32m"},
    {color::yellow, "\033[1;33m"},
    {color::blue, "\033[1;34m"},
    {color::magenta, "\033[1;35m"},
    {color::cyan, "\033[1;36m"},
    {color::reset, "\033[0m"}
};

class command
{
public:
    virtual void execute(env& env) = 0;
    virtual ~command() = default;
};

// arg_parser
bool parse(int argc, char* argv[], env &env);

