#ifndef ROUTING_PROFILE_INDEX_FACTORY_H
#define ROUTING_PROFILE_INDEX_FACTORY_H

#include "routing/profile/data_index.h"

#include <memory>

namespace routing{
namespace profile{

class IndexFactory{
public:
    void CreateGreenIndex();

    std::unique_ptr<DataIndex> LoadGreenIndex();

};

}
}

#endif // ROUTING_PROFILE_INDEX_FACTORY_H