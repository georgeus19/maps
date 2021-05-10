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

class IGenerator {
public:
    void AddGreenIndex(const std::string& green_index_table, double max_edge_uid, double importance);

    void AddPhysicalLengthIndex(const std::string& length_index_table, double max_edge_uid, double importance);

};

class ProfileGenerator {
public:

    ProfileGenerator(database::DatabaseHelper& d, const std::string& base_graph_table, unsigned_id_type max_edge_uid, double scale_max);

    void AddGreenIndex(const std::string& green_index_table, std::vector<double>&& importance_options);

    void AddPhysicalLengthIndex(const std::string& length_index_table, std::vector<double>&& importance_options);

    std::vector<Profile> Generate();

private:
    struct IndexInfo;
    database::DatabaseHelper& d_;
    std::string base_graph_table_;
    unsigned_id_type max_edge_uid_;
    double scale_max_;

    std::vector<IndexInfo> indices_;

    struct IndexInfo{
        std::unique_ptr<DataIndex> index;
        std::vector<double> importance_options;

        IndexInfo(std::unique_ptr<DataIndex>&& i, std::vector<double> im) : index(std::move(i)), importance_options(std::move(im)) {}
    };

    std::vector<std::vector<double>> GetAllImportances(std::vector<IndexInfo>::iterator it, std::vector<IndexInfo>::iterator end);

};

ProfileGenerator::ProfileGenerator(database::DatabaseHelper& d, const std::string& base_graph_table, unsigned_id_type max_edge_uid, double scale_max)
    : d_(d), base_graph_table_(base_graph_table), max_edge_uid_(max_edge_uid), scale_max_(scale_max), indices_() {}

void ProfileGenerator::AddGreenIndex(const std::string& green_index_table, std::vector<double>&& importance_options) {
    auto&& green_index = std::make_unique<GreenIndex>(d_);
    green_index->Load(green_index_table, max_edge_uid_);
    indices_.emplace_back(std::move(green_index), std::move(importance_options));
}

void ProfileGenerator::AddPhysicalLengthIndex(const std::string& length_index_table, std::vector<double>&& importance_options) {
    auto&& length_index = std::make_unique<PhysicalLengthIndex>(d_);
    length_index->Load(length_index_table, max_edge_uid_);
    indices_.emplace_back(std::move(length_index), std::move(importance_options));
}

std::vector<Profile> ProfileGenerator::Generate() {
    std::vector<Profile> profiles{};
    if (indices_.begin() != indices_.end()) {
        for(auto&& im : GetAllImportances(indices_.begin(), indices_.end())) {
            Profile profile{scale_max_};
            auto in_it = indices_.begin();
            auto im_it = im.begin();
            for(; in_it != indices_.end();  ++in_it, ++im_it) {
                profile.AddIndex(in_it->index.get(), *im_it);
            }
            profiles.push_back(std::move(profile));
        }
    }
    return profiles;
}

std::vector<std::vector<double>> ProfileGenerator::GetAllImportances(std::vector<IndexInfo>::iterator it, std::vector<IndexInfo>::iterator end) {
    auto next = it;
    ++next;
    std::vector<std::vector<double>> result;
    if (next != end) {
        auto&& previous_result = GetAllImportances(next, end);
        for(auto&& importance : it->importance_options) {
            for(auto&& p : previous_result) {
                std::vector<double> v{};
                v.push_back(importance);
                v.insert(v.end(), p.begin(), p.end());
                result.push_back(std::move(v));
            }

        }
    } else {
        for(auto&& importance : it->importance_options) {
            std::vector<double> v{};
            v.push_back(importance);
            result.push_back(std::move(v));
        }
    }
    return result;
}



}
}

#endif //BACKEND_ROUTING_PROFILE_GENERATOR_H