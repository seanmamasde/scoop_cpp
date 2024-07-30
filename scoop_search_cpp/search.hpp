#pragma once

#include <string>
#include <vector>

struct search_match
{
    std::string name{};
    std::string version{};
    std::string bin{};
};

inline std::vector<search_match> matches{};

namespace search
{
    std::vector<search_match> search_bucket(const std::string& query, const std::string& bucket_base);
}
