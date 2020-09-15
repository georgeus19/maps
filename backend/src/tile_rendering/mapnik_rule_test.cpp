/*
 * This is just a test file to see if mapnik is correctly installed and if not to see compilation errors and install mapnik correctly.
 */
#include <iostream>
#include <string>
#include <mapnik/map.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/image.hpp>
#include <mapnik/image_util.hpp>

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

using namespace std;
using namespace mapnik;

int main() {
    mapnik::Map m(256,256);

    parameters p;
    p["type"]="postgis";
    p["host"]="127.0.0.1";
    p["port"]="5432";
    p["table"]="planet_osm_line";
    p["geometry_field"]="way";
    p["dbname"]="gis";
    p["user"]="postgres";
    p["password"]=":))";

    layer lyr("Roads");
#if MAPNIK_VERSION >= 200200
    mapnik::datasource_cache::instance().register_datasources("/usr/local/lib/mapnik/input");
    lyr.set_datasource(datasource_cache::instance().create(p));
#else
    set_datasource(datasource_cache::instance()->create(p));
#endif
    // Roads 3 and 4 (The "grey" roads)
    feature_type_style roads34_style;
    {
        rule r;
        r.set_filter(parse_expression("[CLASS] = 3 or [CLASS] = 4"));
        {
            line_symbolizer line_sym;
            put(line_sym,keys::stroke,color(171,158,137));
            put(line_sym,keys::stroke_width,2.0);
            put(line_sym,keys::stroke_linecap,ROUND_CAP);
            put(line_sym,keys::stroke_linejoin,ROUND_JOIN);
            r.append(std::move(line_sym));
        }
        roads34_style.add_rule(std::move(r));
    }
    m.insert_style("smallroads", std::move(roads34_style));

    // Roads 2 (The thin yellow ones)
    feature_type_style roads2_style_1;
    {
        rule r;
        r.set_filter(parse_expression("[CLASS] = 2"));
        {
            line_symbolizer line_sym;
            put(line_sym,keys::stroke,color(171,158,137));
            put(line_sym,keys::stroke_width,4.0);
            put(line_sym,keys::stroke_linecap,ROUND_CAP);
            put(line_sym,keys::stroke_linejoin,ROUND_JOIN);
            r.append(std::move(line_sym));
        }
        roads2_style_1.add_rule(std::move(r));
    }
    m.insert_style("road-border", std::move(roads2_style_1));

    feature_type_style roads2_style_2;
    {
        rule r;
        r.set_filter(parse_expression("[CLASS] = 2"));
        {
            line_symbolizer line_sym;
            put(line_sym,keys::stroke,color(255,250,115));
            put(line_sym,keys::stroke_width,2.0);
            put(line_sym,keys::stroke_linecap,ROUND_CAP);
            put(line_sym,keys::stroke_linejoin,ROUND_JOIN);
            r.append(std::move(line_sym));
        }
        roads2_style_2.add_rule(std::move(r));
    }
    m.insert_style("road-fill", std::move(roads2_style_2));

    // Roads 1 (The big orange ones, the highways)
    feature_type_style roads1_style_1;
    {
        rule r;
        r.set_filter(parse_expression("[CLASS] = 1"));
        {
            line_symbolizer line_sym;
            put(line_sym,keys::stroke,color(188,149,28));
            put(line_sym,keys::stroke_width,7.0);
            put(line_sym,keys::stroke_linecap,ROUND_CAP);
            put(line_sym,keys::stroke_linejoin,ROUND_JOIN);
            r.append(std::move(line_sym));
        }
        roads1_style_1.add_rule(std::move(r));
    }
    m.insert_style("highway-border", std::move(roads1_style_1));

    feature_type_style roads1_style_2;
    {
        rule r;
        r.set_filter(parse_expression("[CLASS] = 1"));
        {
            line_symbolizer line_sym;
            put(line_sym,keys::stroke,color(242,191,36));
            put(line_sym,keys::stroke_width,5.0);
            put(line_sym,keys::stroke_linecap,ROUND_CAP);
            put(line_sym,keys::stroke_linejoin,ROUND_JOIN);
            r.append(std::move(line_sym));
        }
        roads1_style_2.add_rule(std::move(r));
    }
    m.insert_style("highway-fill", std::move(roads1_style_2));
    lyr.add_style("smallroads");
    lyr.add_style("road-border");
    lyr.add_style("road-fill");
    lyr.add_style("highway-border");
    lyr.add_style("highway-fill");
    m.add_layer(lyr);


    //mapnik::load_map(m, "path/to/file.xml");
    m.zoom_all();
    mapnik::image_rgba8 im(256,256);
    mapnik::agg_renderer<mapnik::image_rgba8> ren(m, im);
    ren.apply();
    mapnik::save_to_file(im, "the_image.png");
    std::cout << "Hello, World!" << std::endl;
}

