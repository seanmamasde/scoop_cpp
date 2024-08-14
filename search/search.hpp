#pragma once

#include <string>
#include <type_traits>
#include <vector>

#include "../scoop_cpp/color.hpp"
#include "../scoop_cpp/command.hpp"
#include "../scoop_cpp/env.hpp"

class search_command final : public command
{
public:
    struct match
    {
        // changed to colorful strings for better output
        dye::colorful<std::string> name;
        std::string version;
        dye::colorful<std::string> bin;
    };

    struct result
    {
        std::string bucket_name;
        std::vector<match> match_results;

        result(std::string name, const std::vector<match>& res)
            : bucket_name(std::move(name)), match_results(res)
        {
        }
    };

    explicit search_command(const std::vector<std::string>& queries);
    ~search_command() override = default;

    // disable copy
    search_command(const search_command&) = delete;
    search_command& operator=(const search_command&) = delete;

    // enable move
    search_command(search_command&&) noexcept = default;
    search_command& operator=(search_command&&) noexcept = default;

    void execute(env &env) override;
    [[nodiscard]] match match_package(const std::string& manifest_path) const;
    [[nodiscard]] std::vector<match>search_bucket(const std::string& bucket_base) const;

private:
    std::string query_;

    // flags
    bool regex_ = false;
};