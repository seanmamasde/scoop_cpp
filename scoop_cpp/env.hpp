#pragma once

#include <string>

class env final
{
public:
    env() { scoop_home_owned(); }
    ~env() = default;

    // disable copy
    env(const env&) = delete;
    env& operator=(const env&) = delete;

    // enable move
    env(env&&) noexcept = default;
    env& operator=(env&&) noexcept = default;

    [[nodiscard]] std::string get_scoop_home() const { return scoop_home_; }
    static std::string home_dir_owned();
    static std::string scoop_config_file_owned(const std::string& home_dir);
    void scoop_home_owned();

private:
    std::string scoop_home_;
};
