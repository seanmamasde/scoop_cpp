#pragma once

#include <string>
#include <vector>

#include "../scoop_cpp/arg_parser.hpp"

class search_command final : public command
{
private:
    std::string query_;

public:
    struct match
    {
        std::string name{};
        std::string version{};
        std::string bin{};
    };

    struct result
    {
        std::string bucket_name{};
        std::vector<match> match_results{};

        result(std::string name, std::vector<match> res)
            : bucket_name(std::move(name)), match_results(std::move(res))
        {
        }
    };

    std::vector<match> matches{};

    explicit search_command(const std::vector<std::string>& vq);
    void execute(env& env) override;

    [[nodiscard]] match match_package(const std::string& manifest_path, const std::string& query) const;
    [[nodiscard]] std::vector<match> search_bucket(const std::string& query, const std::string& bucket_base) const;
};
