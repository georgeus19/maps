#ifndef GRAPH_BUILDER_HIGHWAY_FILTER_H
#define GRAPH_BUILDER_HIGHWAY_FILTER_H

#include <osmium/osm/way.hpp>

namespace graph_builder {

class HighwayFilter{
public: 
    virtual bool FilterHighway(const osmium::TagList& tags) = 0;
};



}
#endif// GRAPH_BUILDER_HIGHWAY_FILTER_H