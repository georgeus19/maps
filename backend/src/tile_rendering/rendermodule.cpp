/*
 * This file implements rendering tiles and contains python bindings for the rendering function.
 * Tiles are saved to the directory specified in arguments.
 * It is not used on the server since it is too slow in comparison with Apache + mod_tile (renderd)
 * but it is a good way to test python bindings and that mapnik is correctly installed.
 *
 * Since it is not used, it contains some hardcoded paths (xml, fonts, inputs).
 * Change them if you want to use this module and generate a new .so file using setupRendering.py
 * script in the backend directory.
 */
#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>

#include <iostream>
#include <string>
#include <mapnik/map.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/image.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/layer.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/symbolizer.hpp>
#include <mapnik/text/placements/dummy.hpp>
#include <mapnik/text/text_properties.hpp>
#include <cmath>

using namespace std;
using namespace mapnik;

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
    string inputs_path = "/usr/local/lib/mapnik/input";
    string fonts_path = "/usr/local/lib/mapnik/fonts/";
    string xml_stylesheet_path = "/home/hrubyk/projects/maps/backend/stylesheet_osm_bright/map_stylesheet.xml";
    mapnik::datasource_cache::instance().register_datasources(inputs_path);
    mapnik::Map m(256, 256);
    m.register_fonts(fonts_path);
    mapnik::load_map(m, xml_stylesheet_path);

    //m.set_aspect_fix_mode(mapnik::Map::ADJUST_BBOX_HEIGHT);
    m.zoom_to_box(Tile2BoudingBox(tile_x, tile_y, zoom));
    //m.zoom_all();

    mapnik::image_rgba8 im(256,256);
    mapnik::agg_renderer<mapnik::image_rgba8> ren(m, im);
    ren.apply();

    mapnik::save_to_file(im, tile_path);
    return 1;
}

/*
 * From down below is the setup for python bindings.
 * Parsing arguments from python and calling our tile_render function.
 */
static PyObject* Render(PyObject* self, PyObject* args) {
    int tile_x;
    int tile_y;
    int tile_z;
    const char* tile_path;
    if (!PyArg_ParseTuple(args, "iiis", &tile_x, &tile_y, &tile_z, &tile_path)) {
        return NULL;
    }
    return Py_BuildValue("i", CRender(tile_x, tile_y, tile_z, string(tile_path)));
}

/*
 * Registering method Render so that it can be called from python.
 */
static PyMethodDef renderMethods[] = {
        {"Render", Render, METH_VARARGS, "Render tile."},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef renderModule = {
        PyModuleDef_HEAD_INIT,
        "renderModule",
        "Module for rendering tiles using mapnik",
        -1,
        renderMethods
};

PyMODINIT_FUNC PyInit_renderModule(void) {
    return PyModule_Create(&renderModule);
}