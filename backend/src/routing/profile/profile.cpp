#include "routing/profile/profile.h"

namespace routing{
namespace profile{

Profile::Profile(double scale_max) : scale_max_(scale_max), properties_(), normalized_(false) {}

void Profile::AddIndex(DataIndex* index, double importance) {
    normalized_ = false;
    properties_.emplace_back(index, importance);
}

void Profile::Normalize() {
    for(auto&& p : properties_) {
        p.index->Normalize(scale_max_);
    }
    normalized_ = true;
}

double Profile::GetLength(unsigned_id_type uid) {
    double length = 0;
    for(auto&& property : properties_) {
        length += property.importance * property.index->Get(uid);
    }
    return length;
}    



}
}