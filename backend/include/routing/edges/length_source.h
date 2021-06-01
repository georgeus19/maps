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

class ProfileLengthSource{
public:

    ProfileLengthSource(std::reference_wrapper<profile::Profile> profile) : profile_(profile) {}

    double GetLength(unsigned_id_type id) const {
        return profile_.get().GetLength(id);
    }

private:
    std::reference_wrapper<profile::Profile> profile_;
};



}
#endif // ROUTING_EDGES_LENGTH_SOURCE_H