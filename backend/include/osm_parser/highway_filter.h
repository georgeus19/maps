#ifndef OSM_PARSER_HIGHWAY_FILTER_H
#define OSM_PARSER_HIGHWAY_FILTER_H

#include <osmium/osm/way.hpp>

namespace osm_parser{

class HighwayFilter{
public: 
    virtual bool FilterHighway(const osmium::TagList& tags) = 0;
};



}
#endif// OSM_PARSER_HIGHWAY_FILTER_H