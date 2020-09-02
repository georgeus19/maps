#include <iostream>
#include <string>
#include <mapnik/map.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/image.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/version.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/layer.hpp>

#include <mapnik/rule.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/symbolizer.hpp>
#include <mapnik/text/placements/dummy.hpp>
#include <mapnik/text/text_properties.hpp>
#include <mapnik/expression.hpp>
#include <mapnik/color_factory.hpp>
#include <mapnik/color.hpp>

using namespace std;
using namespace mapnik;

int main() {
    mapnik::datasource_cache::instance().register_datasources("/usr/local/lib/mapnik/input");
    mapnik::Map m(600,300);
    // m.set_background(parse_color("steelblue"));
    // feature_type_style s;
    //rule r;
    // polygon_symbolizer polygon_symbolizer;
    // put(polygon_symbolizer, keys::fill(10));

    mapnik::load_map(m, "/home/hrubyk/projects/maps/backend/world_style.xml");
    m.zoom_all();
    mapnik::image_rgba8 im(256,256);
    mapnik::agg_renderer<mapnik::image_rgba8> ren(m, im);
    ren.apply();
    mapnik::save_to_file(im, "/home/hrubyk/projects/maps/backend/the_image.png");
}
