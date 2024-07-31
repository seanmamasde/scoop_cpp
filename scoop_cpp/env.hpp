#pragma once

#include <string>

class env
{
private:
    std::string scoop_home_{};

public:
    env() { scoop_home_owned(); }
    ~env() = default;

    [[nodiscard]] std::string get_scoop_home() const noexcept { return scoop_home_; }
    static std::string home_dir_owned();
    static std::string scoop_config_file_owned(const std::string& home_dir) noexcept;
    void scoop_home_owned();
};
