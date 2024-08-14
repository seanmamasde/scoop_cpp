#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <future>
#include <iosfwd>
#include <iostream>
#include <regex>
#include <string>
#include <type_traits>
#include <vector>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "search.hpp"
#include "../scoop_cpp/color.hpp"
#include "../scoop_cpp/env.hpp"
#include "../scoop_cpp/thread_pool.hpp"


search_command::search_command(const std::vector<std::string>& queries)
{
    // conform flags and query
    for (auto& q : queries)
    {
        if (q.starts_with("--"))
        {
            // flags
            if (q.substr(2) == "regex")
            {
                regex_ = true;
            }
            else
            {
                std::cerr
                    << dye::yellow("Ignoring unknown flag: ")
                    << dye::yellow(q)
                    << "\n";
            }
        }
        else
        {
            // query
            if (query_.empty())
            {
                query_ = q;
                std::ranges::transform(query_.begin(), query_.end(), query_.begin(), tolower);
            }
            else
            {
                std::cerr
                    << dye::yellow("Ignoring extra query: ")
                    << dye::yellow(q)
                    << "\n";
            }
        }
    }
}

search_command::match search_command::match_package(const std::string& manifest_path) const
{
    // read manifest file
    std::ifstream manifest_file(manifest_path);
    if (!manifest_file)
    {
        std::cerr
            << dye::yellow("Failed to open manifest file: ")
            << dye::yellow(manifest_path)
            << "\n";
        return {};
    }
    nlohmann::json manifest{};
    manifest_file >> manifest;

    // extract package name and version
    const std::string version = manifest.value("version", "");
    std::string lower_name = std::filesystem::path(manifest_path).stem().string();
    std::ranges::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), tolower);

    using colorful_string = dye::colorful<std::string>;

    auto match_and_colorize = [this](const std::string& input, bool& has_matched)-> colorful_string
        {
            colorful_string result;
            if (regex_)
            {
                std::regex pattern(query_);
                std::sregex_iterator begin(input.begin(), input.end(), pattern);
                std::sregex_iterator end;

                size_t last_pos = 0;
                for (auto it = begin; it != end; ++it)
                {
                    has_matched = true;
                    std::smatch match = *it;
                    // append the part of the string before the match
                    result += input.substr(last_pos, match.position() - last_pos);
                    // append the match itself
                    result += dye::green(match.str());
                    // update the last position
                    last_pos = match.position() + match.length();
                }
                // append the rest of the string
                result += input.substr(last_pos);
            }
            else
            {
                size_t pos = 0;
                size_t last_pos = 0;
                while ((pos = input.find(query_, last_pos)) != std::string::npos)
                {
                    has_matched = true;
                    // append the part of the string before the match
                    result += input.substr(last_pos, pos - last_pos);
                    // append the match itself
                    result += dye::green(input.substr(pos, query_.size()));
                    // update the last position
                    last_pos = pos + query_.size();
                }
                // append the rest of the string
                result += input.substr(last_pos);
            }

            return result;
        };

    // no query or query matched name
    bool has_matched = false;
    const auto matched = match_and_colorize(lower_name, has_matched);
    if (query_.empty() || has_matched)
    {
        return { matched, version, colorful_string() };
    }

    // check binaries
    if (manifest.contains("bin"))
    {
        if (manifest["bin"].is_string())
        {
            bool bin_has_matched = false;
            const auto binary = match_and_colorize(manifest["bin"], bin_has_matched);
            if (bin_has_matched)
            {
                return { colorful_string(lower_name), version, binary };
            }
        }
        else if (manifest["bin"].is_array())
        {
            for (const auto& bin : manifest["bin"])
            {
                if (bin.is_string())
                {
                    bool bin_has_matched = false;
                    const auto binary = match_and_colorize(bin, bin_has_matched);
                    if (bin_has_matched)
                    {
                        return { colorful_string(lower_name), version, binary };
                    }
                }
            }
        }
    }

    return {};
}

std::vector<search_command::match> search_command::search_bucket(const std::string& bucket_base) const
{
    auto get_package_names = [this](const std::string& path) -> std::vector<std::string>
        {
            std::vector<std::string> package_names;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".json")
                {
                    package_names.push_back(entry.path().string());
                }
            }

            return package_names;
        };

    std::vector<match> result;

    std::vector<std::string> package_names = get_package_names(bucket_base + "/bucket");
    if (package_names.empty())
    {
        package_names = get_package_names(bucket_base);
    }

    thread_pool pool;
    std::vector<std::future<match>> futures;
    futures.reserve(package_names.size());
    for (const auto& name : package_names)
    {
        futures.push_back(pool.enqueue([this, &name]
            {
                return this->match_package(name);
            }));
    }
    for (auto& fut : futures)
    {
        if (match match = fut.get(); !match.name.empty())
            result.push_back(match);
    }

    std::ranges::sort(result.begin(), result.end(), [](const match& a, const match& b)
        {
            return a.name < b.name;
        });

    return result;
}

void search_command::execute(env& env)
{
    // search all buckets
    std::vector<result> results;
    const std::string buckets_path = env.get_scoop_home() + "/buckets";
    for (const auto& entry : std::filesystem::directory_iterator(buckets_path))
    {
        if (!entry.is_directory())
        {
            continue;
        }

        std::string bucket_base = entry.path().string();
        auto result = search_bucket(bucket_base);
        results.emplace_back(entry.path().filename().string(), std::move(result));
    }

    // print results
    bool has_matches = false;
    for (const auto& result : results)
    {
        if (result.match_results.empty())
        {
            continue;
        }

        has_matches = true;
        std::cout << "\"" << result.bucket_name << "\" bucket:\n";
        for (const auto& [name, version, bin] : result.match_results)
        {
            std::cout << "    " << name << " (" << version << ")";
            if (!bin.empty())
                std::cout << "  -->  includes \"" << bin << "\"";
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    if (!has_matches)
    {
        std::cout << dye::yellow("No matches found.\n");
    }
}
