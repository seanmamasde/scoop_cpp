#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../search/search.hpp"
#include "command.hpp"
#include "env.hpp"

class parser
{
public:
    using command_ptr = std::unique_ptr<command>;
    using command_map = std::unordered_map<std::string, std::function<command_ptr()>>;

    parser(int argc, char* argv[], env& env);
    ~parser() = default;

    // disable copy
    parser(const parser&) = delete;
    parser& operator=(const parser&) = delete;

    // enable move
    parser(parser&&) noexcept = default;
    parser& operator=(parser&&) noexcept = default;

private:
    std::vector<std::string> argv_;

    command_map command_map_ =
    {
        { "search", [this]() -> command_ptr
            {
                return std::make_unique<search_command>(this->argv_);
            }
        }
    };
};