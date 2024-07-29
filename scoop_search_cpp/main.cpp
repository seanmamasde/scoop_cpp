#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include "args.hpp"
#include "env.hpp"
#include "search.hpp"

namespace fs = std::filesystem;

struct search_result
{
    std::string bucket_name{};
    std::vector<search_match> result{};

    search_result(std::string name, std::vector<search_match> res)
        : bucket_name(std::move(name)), result(std::move(res))
    {
    }
};

bool print_results(const std::vector<search_result>& results)
{
    bool has_matches{ false };

    for (const auto& result : results)
    {
        if (result.result.empty())
        {
            continue;
        }
        has_matches = true;
        std::cout << "'" << result.bucket_name << "' bucket:\n";
        for (const auto& [name, version, bin] : result.result)
        {
            std::cout << "    " << name << " (" << version << ")";
            if (!bin.empty())
            {
                std::cout << " --> includes '" << bin << "'";
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }

    if (!has_matches)
    {
        std::cout << "No matches found.\n";
    }

    return has_matches;
}

int main(const int argc, char* argv[])
{                                 
    // speedup blackmagic
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    args args{};
    if (!args.parse(argc, argv))
    {
        throw std::runtime_error("Invalid arguments");
    }
    if (args.hook)
    {
        std::cout << posh_hook << "\n";
        return 0;
    }

    std::string query = args.query;
    std::transform(query.begin(), query.end(), query.begin(), tolower);

    std::string scoop_home{};
    try
    {
        scoop_home = env::scoop_home_owned();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Could not establish scoop home directory: " << e.what()
            << "\n";
        return 1;
    }

    std::vector<search_result> results{};
    try
    {
        const std::string buckets_path = scoop_home + "/buckets";
        for (const auto& entry : fs::directory_iterator(buckets_path))
        {
            if (!entry.is_directory())
            {
                continue;
            }

            std::string bucket_base = entry.path().string();

            auto result = search::search_bucket(query, bucket_base);
            results.emplace_back(entry.path().filename().string(), std::move(result));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error searching for buckets: " << e.what() << "\n";
        return 1;
    }

    const bool has_matches = print_results(results);
    return has_matches ? 0 : 1;
}
