#ifndef ROUTING_EDGES_LENGTH_SOURCE_H
#define ROUTING_EDGES_LENGTH_SOURCE_H

#include "routing/profile/profile.h"

namespace routing{

class NumberLengthSource{
public:
    NumberLengthSource(double length) : length_(length) {}

    double GetLength(unsigned_id_type id) const {
        return length_;
    }

private:
    double length_;
};

class DynamicLengthSource{
public:
    virtual double GetLength(unsigned_id_type uid) const = 0;
};

class EndpointEdgesLengths : public DynamicLengthSource{
public:

    EndpointEdgesLengths() : lengths_() {}

    double GetLength(unsigned_id_type uid) const override {
        return lengths_.at(uid);
    }

    void AddLength(unsigned_id_type id, double length) {
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
    std::vector<double> lengths_;
};


class ProfileEnvelope : public DynamicLengthSource{
public:
    ProfileEnvelope(profile::Profile&& profile) : profile_(std::move(profile)) {}

    // ProfileEnvelope(const ProfileEnvelope& other) = delete;
    // ProfileEnvelope(ProfileEnvelope&& other) = delete;
    // ProfileEnvelope& operator=(const ProfileEnvelope& other) = delete;
    // ProfileEnvelope& operator=(ProfileEnvelope&& other) = delete;
    // ~ProfileEnvelope() = default;

    void SwitchProfile(profile::Profile&& profile) {
        profile_ = std::move(profile);
    }

    profile::Profile& get_profile() {
        return profile_;
    }

    double GetLength(unsigned_id_type id) const override {
        return profile_.GetLength(id);
    }
private:
    profile::Profile profile_;
};

class ProfileLengthSource{
public:
    ProfileLengthSource(DynamicLengthSource* profile) : profile_(profile) {}

    double GetLength(unsigned_id_type id) const {
        return profile_->GetLength(id);
    }

private:
    DynamicLengthSource* profile_;
};



}
#endif // ROUTING_EDGES_LENGTH_SOURCE_H