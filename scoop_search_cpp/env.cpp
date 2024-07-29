#include "env.hpp"
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace env
{
    std::string home_dir_owned()
    {
        // const char* user_profile{ std::getenv("USERPROFILE") };
        char* user_profile{};
        size_t len{ 0 };
        if (_dupenv_s(&user_profile, &len, "USERPROFILE") != 0)
        {
            throw std::runtime_error("USERPROFILE environment variable is undefined");
        }

        return std::string{ user_profile };
    }

    std::string scoop_config_file_owned(const std::string& home_dir)
    {
        // const char* xdg_config_home{ std::getenv("XDG_CONFIG_HOME") };
        char* xdg_config_home{};
        size_t len{ 0 };
        if (_dupenv_s(&xdg_config_home, &len, "XDG_CONFIG_HOME") != 0)
        {
            xdg_config_home = nullptr;
        }

        std::string system_config{};
        if (xdg_config_home)
        {
            system_config = xdg_config_home;
        }
        else
        {
            system_config = home_dir + "./config";
        }

        return system_config;
    }

    std::string scoop_home_owned()
    {
        // const char* scoop{ std::getenv("SCOOP") };
        char* scoop{};
        size_t len{ 0 };
        if (_dupenv_s(&scoop, &len, "SCOOP") != 0)
        {
            scoop = nullptr;
        }

        std::string scoop_home{};
        if (scoop)
        {
            scoop_home = scoop;
        }
        else
        {
            scoop_home = home_dir_owned();
            const std::string scoop_config_path = scoop_config_file_owned(scoop_home);

            if (std::ifstream config_file(scoop_config_path); config_file)
            {
                nlohmann::json config{};
                config_file >> config;
                if (std::string root_path = config.value("root_path", "");
                    !root_path.empty())
                {
                    return root_path;
                }
            }
            scoop_home += "/scoop";
        }

        return scoop_home;
    }
}
