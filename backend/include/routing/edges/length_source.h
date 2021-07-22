#ifndef ROUTING_EDGES_LENGTH_SOURCE_H
#define ROUTING_EDGES_LENGTH_SOURCE_H

#include "routing/profile/profile.h"
#include "routing/types.h"

namespace routing{

/**
 * NumberLengthSource provides a length value for an edge.
 */
class NumberLengthSource{
public:
    NumberLengthSource(float length) : length_(length) {}

    float GetLength(unsigned_id_type id) const {
        return length_;
    }

private:
    float length_;
};

/**
 * DynamicLengthSource is abstract class for providing length to an edge.
 * The idea is that lengths of one edge can be redefined dynamically.
 */
class DynamicLengthSource{
public:
    virtual float GetLength(unsigned_id_type uid) const = 0;
};

/**
 * Provide edge lengths for endpoint edges which are only temporary
 * and definitely not parts of any profile.
 * 
 * This class is used in combination with ProfileLengthSource.
 */
class EndpointEdgesLengths : public DynamicLengthSource{
public:

    EndpointEdgesLengths() : lengths_() {}

    float GetLength(unsigned_id_type uid) const override {
        return lengths_.at(uid);
    }

    void AddLength(unsigned_id_type id, float length) {
        if (id == lengths_.size()) {
            lengths_.push_back(length);
        } else if (id > lengths_.size()) {
            lengths_.resize(id + 1);
            lengths_[id] = length;
        } else {
            lengths_[id] = length;
        }
    }
private:
    std::vector<float> lengths_;
};

/**
 * ProfileEnvelope is a container for profile instance which can be referenced by edges.
 * Edge lentgths can be changed by switching the profile member.
 */
class ProfileEnvelope : public DynamicLengthSource{
public:
    ProfileEnvelope(profile::Profile&& profile) : profile_(std::move(profile)) {}

    void SwitchProfile(profile::Profile&& profile) {
        profile_ = std::move(profile);
    }

    profile::Profile& get_profile() {
        return profile_;
    }

    float GetLength(unsigned_id_type id) const override {
        return profile_.GetLength(id);
    }
private:
    profile::Profile profile_;
};

/**
 * ProfileLengthSource provides lengths for edges based on a profile
 * which can be dynamically changed.
 */ 
class ProfileLengthSource{
public:
    ProfileLengthSource(DynamicLengthSource* profile) : profile_(profile) {}

    float GetLength(unsigned_id_type id) const {
        return profile_->GetLength(id);
    }

private:
    DynamicLengthSource* profile_;
};



}
#endif // ROUTING_EDGES_LENGTH_SOURCE_H