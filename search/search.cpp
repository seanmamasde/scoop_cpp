#include <algorithm>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>

#include "../scoop_cpp/env.hpp"
#include "../scoop_cpp/threadpool.hpp"
#include "search.hpp"

 search_command::search_command(const std::vector<std::string>& vq)
 {
     query_ = vq.front(); // ignore the rest of the arguments
     if (vq.size() > 1)
         std::cerr << color_map[color::yellow] << "Ignoring the rest of the arguments\n" << color_map[color::reset];
 }

void search_command::execute(env& env)
{
    // process query
    std::transform(query_.begin(), query_.end(), query_.begin(), tolower);

    // search all buckets
    std::vector<result> results{};
    const std::string buckets_path = env.get_scoop_home() + "/buckets";
    for (const auto& entry : std::filesystem::directory_iterator(buckets_path))
    {
        if (!entry.is_directory())
            continue;

        std::string bucket_base = entry.path().string();

        auto result = search_bucket(query_, bucket_base);
        results.emplace_back(entry.path().filename().string(), std::move(result));
    }

    // print results
    bool has_matches{ false };
    for (const auto& result : results)
    {
        if (result.match_results.empty())
            continue;

        has_matches = true;

        std::cout << "'" << result.bucket_name << "' bucket:\n";
        for (const auto& [name, version, bin] : result.match_results)
        {
            std::cout << "    " << name << " (" << version << ")";
            if (!bin.empty())
                std::cout << " --> includes '" << bin << "'";
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    if (!has_matches)
        std::cout << "No matches found.\n";
}

search_command::match search_command::match_package(const std::string& manifest_path, const std::string& query) const
{
    // read manifest file
    std::ifstream manifest_file(manifest_path);
    if (!manifest_file)
    {
        std::cerr << "Failed to open manifest file: " << manifest_path << "\n";
        return {};
    }
    nlohmann::json manifest{};
    manifest_file >> manifest;

    // check if the manifest contains the query
    const std::string version = manifest.value("version", "");
    std::string lower_name = std::filesystem::path(manifest_path).stem().string();
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), tolower);

    auto check_bin = [this](const std::string& bin, const std::string& query) -> bool
        {
            std::string lower_bin{ bin };
            std::transform(lower_bin.begin(), lower_bin.end(), lower_bin.begin(), tolower);
            return lower_bin.find(query) != std::string::npos;
        };

    if (query.empty() || lower_name.find(query) != std::string::npos)
        return { lower_name, version, "" };

    if (manifest.contains("bin"))
    {
        if (manifest["bin"].is_string())
            if (check_bin(manifest["bin"], query))
                return { lower_name, version, manifest["bin"] };

        if (manifest["bin"].is_array())
            for (const auto& bin : manifest["bin"])
                if (bin.is_string() && check_bin(bin, query))
                    return { lower_name, version, bin };
    }
    return {};
}

std::vector<search_command::match> search_command::search_bucket(const std::string& query, const std::string& bucket_base) const
{
    auto get_package_names = [this](const std::string& path) -> std::vector<std::string>
        {
            std::vector<std::string> package_names{};
            for (const auto& entry : std::filesystem::directory_iterator(path))
                if (entry.is_regular_file() && entry.path().extension() == ".json")
                    package_names.push_back(entry.path().string());
            return package_names;
        };

    std::vector<match> result{};

    std::vector<std::string> package_names = get_package_names(bucket_base + "/bucket");
    if (package_names.empty())
        package_names = get_package_names(bucket_base);

    threadpool pool{};
    std::vector<std::future<match>> futures;
    futures.reserve(package_names.size());
    for (const auto& name : package_names)
        futures.push_back(pool.enqueue([this, &name, &query]
            {
                return this->match_package(name, query);
            }));
    for (auto& fut : futures)
        if (match match = fut.get(); !match.name.empty())
            result.push_back(match);

    std::sort(result.begin(), result.end(), [](const match& a, const match& b)
        {
            return a.name < b.name;
        });

    return result;
}
