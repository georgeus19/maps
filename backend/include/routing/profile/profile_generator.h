#ifndef ROUTING_PROFILE_PROFILE_GENERATOR_H
#define ROUTING_PROFILE_PROFILE_GENERATOR_H

#include "routing/profile/profile.h"
#include "routing/profile/preference_index.h"
#include "routing/profile/green_index.h"
#include "routing/types.h"

#include "routing/database/database_helper.h"

#include <exception>
#include <string>
#include <vector>
#include <memory>

namespace routing {
namespace profile{

class ProfileGenerator {
public:

    ProfileGenerator(const std::shared_ptr<PreferenceIndex>& index);

    void AddIndex(const std::shared_ptr<PreferenceIndex>& index, std::vector<float>&& importance_options);

    std::vector<Profile> Generate();

    Profile GetFrontProfile();

private:
    struct IndexInfo;

    std::shared_ptr<PreferenceIndex> base_index_;

    std::vector<IndexInfo> indices_;

    struct IndexInfo{
        std::shared_ptr<PreferenceIndex> index;
        std::vector<float> importance_options;

        IndexInfo(std::shared_ptr<PreferenceIndex> i, std::vector<float> im) : index(i), importance_options(std::move(im)) {}
    };

    std::vector<std::vector<float>> GetAllImportances(std::vector<IndexInfo>::iterator it, std::vector<IndexInfo>::iterator end);

};

ProfileGenerator::ProfileGenerator(const std::shared_ptr<PreferenceIndex>& index) : base_index_(index), indices_() {}

void ProfileGenerator::AddIndex(const std::shared_ptr<PreferenceIndex>& index, std::vector<float>&& importance_options) {
    indices_.emplace_back(index, std::move(importance_options));
}

std::vector<Profile> ProfileGenerator::Generate() {
    std::vector<Profile> profiles{};
    if (indices_.begin() != indices_.end()) {
        for(auto&& im : GetAllImportances(indices_.begin(), indices_.end())) {
            Profile profile{base_index_};
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

Profile ProfileGenerator::GetFrontProfile() {
    Profile profile{base_index_};
    for(auto&& index_info : indices_) {
        profile.AddIndex(index_info.index, index_info.importance_options.front());
    }
    return profile;
}

std::vector<std::vector<float>> ProfileGenerator::GetAllImportances(std::vector<IndexInfo>::iterator it, std::vector<IndexInfo>::iterator end) {
    auto next = it;
    ++next;
    std::vector<std::vector<float>> result;
    if (next != end) {
        auto&& previous_result = GetAllImportances(next, end);
        for(auto&& importance : it->importance_options) {
            for(auto&& p : previous_result) {
                std::vector<float> v{};
                v.push_back(importance);
                v.insert(v.end(), p.begin(), p.end());
                result.push_back(std::move(v));
            }
        }
    } else {
        for(auto&& importance : it->importance_options) {
            std::vector<float> v{};
            v.push_back(importance);
            result.push_back(std::move(v));
        }
    }
    return result;
}




}
}
#endif //ROUTING_PROFILE_PROFILE_GENERATOR_H