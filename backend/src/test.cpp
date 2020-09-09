#include "database/database_helper.h"
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
#include <cmath>

using namespace std;
using namespace mapnik;
const string kDbName = "gis";
const string kUser = "postgres";
const string kPassword = "wtz2trln";
const string kHostAddress = "127.0.0.1";
const string kPort = "5432";

int long2tilex(double lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * (1 << z)));
}

int lat2tiley(double lat, int z)
{
    double latrad = lat * M_PI/180.0;
    return (int)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z)));
}

double tilex2long(int x, int z)
{
    return x / (double)(1 << z) * 360.0 - 180;
}

double tiley2lat(int y, int z)
{
    double n = M_PI - 2.0 * M_PI * y / (double)(1 << z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

box2d<double> Tile2BoudingBox(int tile_x, int tile_y, int zoom) {
    double x0 = tilex2long(tile_x, zoom);
    double x1 = tilex2long(tile_x + 1, zoom);
    double y0 = tiley2lat(tile_y, zoom);
    double y1 = tiley2lat(tile_y + 1, zoom);

    lonlat2merc(&x0, &y0, 1);
    lonlat2merc(&x1, &y1, 1);
    return box2d<double>(x0, y0, x1, y1);
}

int CRender(int tile_x, int tile_y, int zoom, const string& tile_path) {
    std::cout << tile_x << "," << tile_y << "," << zoom << "," << tile_path << std::endl;
    mapnik::datasource_cache::instance().register_datasources("/usr/local/lib/mapnik/input");
    mapnik::Map m(256, 256);
    m.register_fonts("/usr/local/lib/mapnik/fonts/");
    mapnik::load_map(m, "/home/hrubyk/projects/maps/backend/stylesheet_osm_bright/map_stylesheet.xml");

    //m.set_aspect_fix_mode(mapnik::Map::ADJUST_BBOX_HEIGHT);
    m.zoom_to_box(Tile2BoudingBox(tile_x, tile_y, zoom));
    //m.zoom_all();

    mapnik::image_rgba8 im(256,256);
    mapnik::agg_renderer<mapnik::image_rgba8> ren(m, im);
    ren.apply();
    /*
    std::ostringstream oss;
    oss << "/home/hrubyk/projects/maps/api/";
    oss << "im_" << zoom << "_" << (int)x << "_" << (int)y << ".png";
    std::string image_path = oss.str();
    */
    mapnik::save_to_file(im, tile_path);
    return 1;
}

int main() {
    return CRender(2197, 1392, 12, "/home/hrubyk/projects/maps/test_tile.png");
/*
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
*/
/*
    parameters p;
    p["type"]="postgis";
    p["host"]="127.0.0.1";
    p["port"]="5432";
    p["table"]="planet_osm_line, planet_osm_polygon, planet_osm_point";
    p["geometry_field"]="way";
    p["dbname"]="gis_stylesheet";
    p["user"]="postgres";
    p["password"]="wtz2trln";


    mapnik::datasource_cache::instance().create(p);
*/
    mapnik::datasource_cache::instance().register_datasources("/usr/local/lib/mapnik/input");
    //set_datasource(datasource_cache::instance().create(p));
    mapnik::Map m(600,600);
    m.register_fonts("/usr/local/lib/mapnik/fonts/");
    mapnik::load_map(m, "/home/hrubyk/projects/maps/backend/stylesheet_osm_bright/map_stylesheet.xml");
    //mapnik::load_map(m, "/home/hrubyk/projects/maps/backend/world_style.xml");



#if MAPNIK_VERSION >= 200200

#else
    set_datasource(datasource_cache::instance()->create(p));
#endif
    double x = 13.373637;
    double y = 49.738431;
    if (mapnik::lonlat2merc(&x, &y, 1) == false) {
        std::cout << "longlat2merc failed\n";
        return 1;
    }
    std::cout << x << " " << y << std::endl;
    double width = 30;
    size_t zoom = 9;
    double dx = ((20037508.34*2*(width/2)))/(256*(exp2(zoom)));
    mapnik::box2d<double> box(x-dx, y-10, x+dx, y+10);
    m.set_aspect_fix_mode(mapnik::Map::ADJUST_BBOX_HEIGHT);
    m.zoom_to_box(box);
    //m.zoom_all();

    mapnik::image_rgba8 im(600,600);
    mapnik::agg_renderer<mapnik::image_rgba8> ren(m, im);
    ren.apply();
    mapnik::save_to_file(im, "/home/hrubyk/projects/maps/backend/im.png");
    /*
    DatabaseHelper d(kDbName, kUser, kPassword, kHostAddress, kPort);
    if (d.IsDbOpen()) {
        std::cout << "db is open" << std::endl;
        d.SearchFor("Haus Drei");
    }
    else {
        std::cout << "db is not open";
    }
    return 0;
    */
}