#include "routing/profile/profile.h"
#include "routing/types.h"

namespace routing{
namespace profile{

Profile::Profile() : base_index_(), properties_() {}

Profile::Profile(const std::shared_ptr<PreferenceIndex>& base_index) : base_index_(base_index), properties_() {}

void Profile::AddIndex(const std::shared_ptr<PreferenceIndex>& index, float importance) {
    properties_.emplace_back(index, importance);
}

std::string Profile::GetName() const {
    std::string name = "";
    for(auto&& prop : properties_) {
        name += prop.index->GetName();
        std::string importance = std::to_string(prop.importance);
        for(auto&& d : importance) {
            if (d == '.') {
                d = 'd';
            }
            if (d == '-') {
                d = '_';
            }
        }
        name += std::move(importance);
    }
    return name;
}

float Profile::GetLength(unsigned_id_type uid) const {
    // std::cout << "PROFILE GET LENGTH:" << std::endl;
    float base_index_value = base_index_->Get(uid);
    // std::cout << base_index_->GetName() << " " << base_index_->Get(uid) << " " << base_index_->GetInverted(uid) << std::endl;
    float length = base_index_value;

    for(auto&& property : properties_) {
        float index_value;
        float importance;
        if (property.importance >= 0) {
            index_value = property.index->Get(uid);
            importance = property.importance;
        } else {
            index_value = property.index->GetInverted(uid);
            importance = -property.importance;
        }
        // std::cout << property.index->GetName() << " " << property.index->Get(uid) << " " << property.index->GetInverted(uid) << " " << property.importance << " " << std::endl;
        
        length += importance * index_value * base_index_value;
    }
    return length;
}    

const std::shared_ptr<PreferenceIndex>& Profile::GetBaseIndex() {
    return base_index_;
}

const std::shared_ptr<PreferenceIndex>& Profile::GetIndex(const std::string& name) {
        auto it = std::find_if(properties_.begin(), properties_.end(), [&](const Property& p){
            return p.index->GetName() == name;
        });
        if (it != properties_.end()) {
            return it->index;
        } else {
            throw InvalidArgumentException{"Index " + name + " not found in profile."};
        }
    }




}
}