#include "routing/profile/profile.h"

namespace routing{
namespace profile{

Profile::Profile(double scale_max) : scale_max_(scale_max), properties_(), normalized_(false) {}

void Profile::AddIndex(const std::shared_ptr<DataIndex>& index, int32_t importance) {
    normalized_ = false;
    properties_.emplace_back(index, importance);
}

void Profile::Normalize() {
    for(auto&& p : properties_) {
        p.index->Normalize(scale_max_);
    }
    normalized_ = true;
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
        length += static_cast<double>(property.importance) * property.index->Get(uid);
    }
    return length;
}    



}
}