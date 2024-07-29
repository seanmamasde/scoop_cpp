#pragma once

#include <string>

namespace env
{
    std::string home_dir_owned();

    std::string scoop_config_file_owned(const std::string& home_dir);

    std::string scoop_home_owned();
}
