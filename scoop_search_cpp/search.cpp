#include "search.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include "threadpool.hpp"

namespace search
{
    std::vector<std::string> get_package_names(const std::string& path)
    {
        std::vector<std::string> package_names{};
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                package_names.push_back(entry.path().string());
            }
        }
        return package_names;
    }

    bool check_bin(const std::string& bin, const std::string& query)
    {
        std::string lower_bin{ bin };
        std::transform(lower_bin.begin(), lower_bin.end(), lower_bin.begin(), tolower);
        return lower_bin.find(query) != std::string::npos;
    }

    search_match match_package(const std::string& manifest_path, const std::string& query)
    {
        std::ifstream manifest_file(manifest_path);
        if (!manifest_file)
        {
            std::cerr << "Failed to open manifest file: " << manifest_path << "\n";
            return {};
        }

        nlohmann::json manifest{};
        manifest_file >> manifest;

        const std::string version = manifest.value("version", "");
        std::string lower_name = std::filesystem::path(manifest_path).stem().string();
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), tolower);

        if (query.empty() || lower_name.find(query) != std::string::npos)
        {
            return { lower_name, version, "" };
        }

        if (manifest.contains("bin"))
        {
            if (manifest["bin"].is_string())
            {
                if (check_bin(manifest["bin"], query))
                {
                    return { lower_name, version, manifest["bin"] };
                }
            }
            else if (manifest["bin"].is_array())
            {
                for (const auto& bin : manifest["bin"])
                {
                    if (bin.is_string() && check_bin(bin, query))
                    {
                        return { lower_name, version, bin };
                    }
                }
            }
        }
        return {};
    }

    std::vector<search_match> search_bucket(const std::string& query, const std::string& bucket_base)
    {
        std::vector<search_match> result{};

        std::vector<std::string> package_names = get_package_names(bucket_base + "/bucket");
        if (package_names.empty())
        {
            package_names = get_package_names(bucket_base);
        }

        threadpool pool{};
        std::vector<std::future<search_match>> futures;
        futures.reserve(package_names.size());
        for (const auto& name : package_names)
        {
            futures.push_back(pool.enqueue(match_package, name, query));
        }
        for (auto& fut : futures)
        {
            if (search_match match = fut.get(); !match.name.empty())
            {
                result.push_back(match);
            }
        }

        std::sort(result.begin(), result.end(), [](const search_match& a, const search_match& b)
            {
                return a.name < b.name;
            });

        return result;
    }
}
