#ifndef BACKEND_ROUTING_PROFILE_GENERATOR_H
#define BACKEND_ROUTING_PROFILE_GENERATOR_H

#include "routing/profile/profile.h"
#include "routing/profile/data_index.h"
#include "routing/profile/green_index.h"

#include "database/database_helper.h"

#include <exception>
#include <string>
#include <vector>
#include <memory>

namespace routing {
namespace profile{

class ProfileGenerator {
public:

    ProfileGenerator(database::DatabaseHelper& d, const std::string& base_graph_table);

    void AddIndex(std::shared_ptr<DataIndex> index, std::vector<int32_t>&& importance_options);

    std::vector<Profile> Generate();

private:
    struct IndexInfo;
    database::DatabaseHelper& d_;
    std::string base_graph_table_;

    std::vector<IndexInfo> indices_;

    struct IndexInfo{
        std::shared_ptr<DataIndex> index;
        std::vector<int32_t> importance_options;

        IndexInfo(std::shared_ptr<DataIndex> i, std::vector<int32_t> im) : index(i), importance_options(std::move(im)) {}
    };

    std::vector<std::vector<int32_t>> GetAllImportances(std::vector<IndexInfo>::iterator it, std::vector<IndexInfo>::iterator end);

};

ProfileGenerator::ProfileGenerator(database::DatabaseHelper& d, const std::string& base_graph_table)
    : d_(d), base_graph_table_(base_graph_table), indices_() {}

void ProfileGenerator::AddIndex(std::shared_ptr<DataIndex> index, std::vector<int32_t>&& importance_options) {
    indices_.emplace_back(index, std::move(importance_options));
}

std::vector<Profile> ProfileGenerator::Generate() {
    std::vector<Profile> profiles{};
    if (indices_.begin() != indices_.end()) {
        for(auto&& im : GetAllImportances(indices_.begin(), indices_.end())) {
            Profile profile{};
            auto in_it = indices_.begin();
            auto im_it = im.begin();
            for(; in_it != indices_.end();  ++in_it, ++im_it) {
                profile.AddIndex(in_it->index, *im_it);
            }
            profiles.push_back(std::move(profile));
        }
    }
    return profiles;
}

std::vector<std::vector<int32_t>> ProfileGenerator::GetAllImportances(std::vector<IndexInfo>::iterator it, std::vector<IndexInfo>::iterator end) {
    auto next = it;
    ++next;
    std::vector<std::vector<int32_t>> result;
    if (next != end) {
        auto&& previous_result = GetAllImportances(next, end);
        for(auto&& importance : it->importance_options) {
            for(auto&& p : previous_result) {
                std::vector<int32_t> v{};
                v.push_back(importance);
                v.insert(v.end(), p.begin(), p.end());
                result.push_back(std::move(v));
            }

        }
    } else {
        for(auto&& importance : it->importance_options) {
            std::vector<int32_t> v{};
            v.push_back(importance);
            result.push_back(std::move(v));
        }
    }
    return result;
}



}
}

#endif //BACKEND_ROUTING_PROFILE_GENERATOR_H