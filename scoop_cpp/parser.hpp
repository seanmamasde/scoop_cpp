#pragma once

#include <memory>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "env.hpp"
#include "command.hpp"
#include "../search/search.hpp"

class parser
{
public:
    using command_ptr = std::unique_ptr<command>;

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
    const std::unordered_map<std::string, std::function<std::unique_ptr<command>()>> command_map_ =
    { { "search", [this]() -> command_ptr
            {
                return std::make_unique<search_command>(this->argv_);
            }
        }
    };
};