#pragma once

#include "env.hpp"

class command
{
public:
    command() = default;
    virtual ~command() = default;

    // virtual method to execute the command
    virtual void execute(env &env) = 0;

    // disable copy
    command(const command&) = delete;
    command& operator=(const command&) = delete;

    // enable move
    command(command&&) noexcept = default;
    command& operator=(command&&) noexcept = default;
};
