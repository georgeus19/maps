#ifndef ROUTING_EDGE_FACTORY_H
#define ROUTING_EDGE_FACTORY_H

#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_edge.h"
#include "routing/edges/length_source.h"
#include "routing/types.h"

#include "routing/profile/profile.h"

namespace routing{

/**
 * This file contains factories for all types of edges for each LengthSource: NumberLengthSource and ProfileLengthSource.
 * They generally require a template input class that provides all properties necessary for given edge type.
 */

template <typename LS>
class EdgeTypeResolver{
public:
    typename BasicEdge<LS>::EdgeType GetEdgeType(bool undirected) {
        if (undirected) {
            return BasicEdge<LS>::EdgeType::twoway;
        } else {
            return BasicEdge<LS>::EdgeType::forward;
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
            EdgeTypeResolver<NumberLengthSource>{}.GetEdgeType(input.GetUndirected())};
    }
};

class CHNumberEdgeFactory{
public:
    using Edge = CHEdge<NumberLengthSource>; 

    CHNumberEdgeFactory() {}

    template <typename Input>
    Edge Create(const Input& input) {
        return Edge{input.GetUid(), input.GetFrom(), input.GetTo(), input.GetLength(),
            EdgeTypeResolver<NumberLengthSource>{}.GetEdgeType(input.GetUndirected()), input.GetContractedVertex()};
    }
};

class BasicProfileEdgeFactory{
public:
    using Edge = BasicEdge<ProfileLengthSource>; 

    BasicProfileEdgeFactory(DynamicLengthSource* length_source) : length_source_(length_source) {}

    template <typename Input>
    Edge Create(const Input& input) {
        return Edge{input.GetUid(), input.GetFrom(), input.GetTo(), ProfileLengthSource{length_source_},
            EdgeTypeResolver<ProfileLengthSource>{}.GetEdgeType(input.GetUndirected())};
    }
private:
    DynamicLengthSource* length_source_;
};

class CHProfileEdgeFactory{
public:
    using Edge = CHEdge<ProfileLengthSource>;

    CHProfileEdgeFactory(DynamicLengthSource* length_source) : length_source_(length_source) {}

    template <typename Input>
    Edge Create(const Input& input) {
        return Edge{input.GetUid(), input.GetFrom(), input.GetTo(), ProfileLengthSource{length_source_},
            EdgeTypeResolver<ProfileLengthSource>{}.GetEdgeType(input.GetUndirected()), input.GetContractedVertex()};
    }
private:
    DynamicLengthSource* length_source_;
};

template <typename E>
class NumberEndpointEdgeFactory{
public:
    using Edge = E;

    NumberEndpointEdgeFactory() {}

    template <typename Input>
    E Create(const Input& input) {
        return E{input.GetUid(), input.GetFrom(), input.GetTo(), input.GetLength()};
    }
};

template <typename E>
class ProfileEndpointEdgeFactory{
public:
    using Edge = E;

    ProfileEndpointEdgeFactory(EndpointEdgesLengths* edge_lengths) : edge_lengths_(edge_lengths) {}

    template <typename Input>
    E Create(const Input& input) {
        edge_lengths_->AddLength(input.GetUid(), input.GetLength());
        return E{input.GetUid(), input.GetFrom(), input.GetTo(), ProfileLengthSource{edge_lengths_}};
    }
private:
    EndpointEdgesLengths* edge_lengths_;
};







}
#endif // ROUTING_EDGE_FACTORY_H