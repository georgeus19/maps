#ifndef BACKEND_PREPROCESSING_EDGE_DATA_MANAGER_H
#define BACKEND_PREPROCESSING_EDGE_DATA_MANAGER_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/data_index.h"
#include "routing/preprocessing/green_index.h"
#include "routing/preprocessing/physical_length_index.h"
#include "routing/exception.h"

#include "database/database_helper.h"

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <limits>

namespace routing {
namespace preprocessing {

class EdgeLengthCalculationManager{
public:
    EdgeLengthCalculationManager(database::DatabaseHelper& d);

    void AddGreenIndex(const std::string& green_index_table, double max_edge_uid, double importance);

    void AddPhysicalLengthIndex(const std::string& length_index_table, double max_edge_uid, double importance);

    void CreateGreenIndex(const std::string& edges_table, const std::string& osm_polygons_table, const std::string& green_index_table);

    void Normalize(double scale_max);

protected:
    struct DataContainer;
    database::DatabaseHelper& d_;
    std::vector<DataContainer> data_;

    struct DataContainer{
        std::unique_ptr<DataIndex> index;
        double importance;

        DataContainer(std::unique_ptr<DataIndex>&& ix, double im) : index(std::move(ix)), importance(im) {}

        DataContainer(const DataContainer& other) = delete;
        DataContainer(DataContainer&& other) = default;
        DataContainer& operator=(const DataContainer& other) = delete;
        DataContainer& operator=(DataContainer&& other) = default;
        ~DataContainer() = default;
    };

    template <typename Index>
    bool IsIndexPresent(DataIndex* index);
};

template <typename Index>
bool EdgeLengthCalculationManager::IsIndexPresent(DataIndex* index) {
    bool present = false;
    for(auto&& c : data_) {
        if (dynamic_cast<Index*>(c.index.get())) {
            present = true;
        }
    }
    return present;
}

EdgeLengthCalculationManager::EdgeLengthCalculationManager(database::DatabaseHelper& d) : d_(d), data_() {}

void EdgeLengthCalculationManager::AddGreenIndex(const std::string& green_index_table, double max_edge_uid, double importance) {
    DataContainer green_container{std::make_unique<GreenIndex>(d_), importance};
    if (IsIndexPresent<GreenIndex>(green_container.index.get())) {
        throw DataIndexAlreadyPresentException{"GreenIndex is already present in EdgeLengthCalculationManager."};
    }
    green_container.index->Load(green_index_table, max_edge_uid);
    data_.push_back(std::move(green_container));
}

void EdgeLengthCalculationManager::AddPhysicalLengthIndex(const std::string& length_index_table, double max_edge_uid, double importance) {
    DataContainer length_container{std::make_unique<PhysicalLengthIndex>(d_), importance};
    if (IsIndexPresent<PhysicalLengthIndex>(length_container.index.get())) {
        throw DataIndexAlreadyPresentException{"PhysicalLengthIndex is already present in EdgeLengthCalculationManager."};
    }
    length_container.index->Load(length_index_table, max_edge_uid);
    data_.push_back(std::move(length_container));
}

void EdgeLengthCalculationManager::CreateGreenIndex(const std::string& edges_table, const std::string& osm_polygons_table, const std::string& green_index_table) {
    GreenIndex index{d_};
    index.Create(edges_table, osm_polygons_table, green_index_table);
}

void EdgeLengthCalculationManager::Normalize(double scale_max) {
    for(auto&& c : data_) {
        c.index->Normalize(scale_max);
    }
}

}
}

#endif //BACKEND_PREPROCESSING_EDGE_DATA_MANAGER_H