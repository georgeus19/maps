#ifndef ROUTING_EDGE_FACTORY_H
#define ROUTING_EDGE_FACTORY_H

#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_edge.h"
#include "routing/edges/length_source.h"

#include "routing/profile/profile.h"

namespace routing{

class EdgeTypeResolver{
public:
    static BasicEdge<NumberLengthSource>::EdgeType GetEdgeType(bool undirected) {
        if (undirected) {
            return BasicEdge<NumberLengthSource>::EdgeType::twoway;
        } else {
            return BasicEdge<NumberLengthSource>::EdgeType::forward;
        }
    }
};

class BasicNumberEdgeFactory{
public:
    using Edge = BasicEdge<NumberLengthSource>; 

    BasicNumberEdgeFactory() {}

    template <typename Input>
    Edge Create(const Input& input) {
        return Edge{input.GetUid(), input.GetFrom(), input.GetTo(), input.GetLength(), 
            EdgeTypeResolver::GetEdgeType(input.GetUndirected())};
    }
};

class CHNumberEdgeFactory{
public:
    using Edge = CHEdge<NumberLengthSource>; 

    CHNumberEdgeFactory() {}

    template <typename Input>
    Edge Create(const Input& input) {
        return Edge{input.GetUid(), input.GetFrom(), input.GetTo(), input.GetLength(),
            EdgeTypeResolver::GetEdgeType(input.GetUndirected()), input.GetContractedVertex()};
    }
};

class BasicProfileEdgeFactory{
public:
    using Edge = BasicEdge<ProfileLengthSource>; 

    BasicProfileEdgeFactory(std::reference_wrapper<profile::Profile> profile) : profile_(profile) {}

    template <typename Input>
    Edge Create(const Input& input) {
        return Edge{input.GetUid(), input.GetFrom(), input.GetTo(), profile_,
            EdgeTypeResolver::GetEdgeType(input.GetUndirected()), input.GetContractedVertex()};
    }
private:
    std::reference_wrapper<profile::Profile> profile_;
};


class CHProfileEdgeFactory{
public:
    using Edge = CHEdge<ProfileLengthSource>;

    CHProfileEdgeFactory(std::reference_wrapper<profile::Profile> profile) : profile_(profile) {}

    template <typename Input>
    Edge Create(const Input& input) {
        return Edge{input.GetUid(), input.GetFrom(), input.GetTo(), profile_,
            EdgeTypeResolver::GetEdgeType(input.GetUndirected()), input.GetContractedVertex()};
    }
private:
    std::reference_wrapper<profile::Profile> profile_;
};







}
#endif // ROUTING_EDGE_FACTORY_H