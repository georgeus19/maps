#include "routing/profile/profile.h"

namespace routing{
namespace profile{

Profile::Profile() : properties_(), normalized_(false) {}

void Profile::AddIndex(const std::shared_ptr<DataIndex>& index, int32_t importance) {
    normalized_ = false;
    properties_.emplace_back(index, importance);
}

void Profile::Normalize(double scale_max) {
    if (!properties_.empty()) {
        for(auto&& p : properties_) {
            p.index->Normalize(scale_max);
        }
        normalized_ = true;
    } else {
        throw InvalidValueException{"Profile must have at least one data index!"};
    }
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
    if (!normalized_) {
        throw InvalidValueException{"Profile is not normalized when GetLength is used."};
    }
    double length = 0;
    for(auto&& property : properties_) {
        length += static_cast<double>(property.importance) * property.index->Get(uid);
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

void Profile::set_normalized() {
    normalized_ = true;
}




}
}