#include "routing/profile/profile.h"

namespace routing{
namespace profile{

Profile::Profile() : properties_() {}

void Profile::AddIndex(const std::shared_ptr<DataIndex>& index, int32_t importance) {
    properties_.emplace_back(index, importance);
}

std::string Profile::GetName() const {
    std::string name = "";
    for(auto&& prop : properties_) {
        name += prop.index->GetName();
        name += std::to_string(prop.importance);
    }
    return name;
}

double Profile::GetLength(unsigned_id_type uid) const {
    double length = 0;
    for(auto&& property : properties_) {
        double index_value;
        int32_t importance;
        if (property.importance >= 0) {
            index_value = property.index->Get(uid);
            importance = property.importance;
        } else {
            index_value = property.index->GetInverted(uid);
            importance = -property.importance;
        }
        length += static_cast<double>(importance) * index_value;
    }
    return length;
}    

std::shared_ptr<DataIndex> Profile::GetIndex(const std::string& name) {
        auto it = std::find_if(properties_.begin(), properties_.end(), [&](const Property& p){
            return p.index->GetName() == name;
        });
        if (it != properties_.end()) {
            return it->index;
        } else {
            return std::shared_ptr<DataIndex>();
        }
    }




}
}