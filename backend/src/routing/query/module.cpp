/*
 * This file contains the routing functionality as well as routing bindings to python.
 * Python bindings are created using python3.8/Python.h.
 */

#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "database/db_graph.h"
#include "database/db_edge_iterator.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "routing/bidirectional_graph.h"
#include "routing/basic_edge_endpoint_handler.h"
#include "routing/query/module.h"

#include <string>

using namespace std;
using namespace routing;
using namespace query;
using namespace database;

/**/
/**
 * Function to compute shortest paths that can be called from python.
 * @param self
 * @param args Arguments for routing.
 * @return geoJson list of geometries of edges that are part of the shortest route all in string.
 */
static PyObject* CalculateShortestRoute(PyObject* self, PyObject* args) {
    const char * table_name;
    double start_lon;
    double start_lat;
    double end_lon;
    double end_lat;
    if (!PyArg_ParseTuple(args, "sdddd", &table_name, &start_lon, &start_lat, &end_lon, &end_lat)) {
        return NULL;
    }

    string result;
    try {
        result = CCalculateShortestRoute<DijkstraSetup>(table_name, utility::Point{start_lon, start_lat}, utility::Point{end_lon, end_lat});
        return Py_BuildValue("s", result.c_str());
    } catch (exception & e) {
        result = "ER" + string{e.what()};
        return Py_BuildValue("s", result.c_str());
    }


}

/**
 * Register `CalculateShortestRoute` as a method that can be called from python.
 */
static PyMethodDef routing_methods[] = {
        {"CalculateShortestRoute", CalculateShortestRoute, METH_VARARGS, "Calculate shortest path between two points in selected table."},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef routing_module = {
        PyModuleDef_HEAD_INIT,
        "routing_module",
        "Module for routing.",
        -1,
        routing_methods
};

PyMODINIT_FUNC PyInit_routing_module(void) {
    return PyModule_Create(&routing_module);
}
/*
//0: Object { lon: 13.391759236865617, lat: 49.725876733293816 }
//1: Object { lon: 13.39411338612247, lat: 49.72563051868181 }
//13.392612138690344, lat: 49.72598770285154 }
//1: Object { lon: 13.394494339635123, lat: 49.72594955709469 }
//0: Object { lon: 13.392649532854506, lat: 49.72567213250674 }
//1: Object { lon: 13.393126720755621, lat: 49.72520397491957 }
//Object { lon: 13.391427565774665, lat: 49.72625472231029 }
//1: Object { lon: 13.393126622053574, lat: 49.725377367144915 }
//15.568861848533453, 50.66747073900733, 15.581821585773252,  50.67989977147217)
int main() {
    string result;
    try { //13.39134574848472, 49.72619577008857 , 13.390991827529778, 49.72644544959591
//        result = CCalculateShortestRoute("czedges", utility::Point{13.3998825, 49.7230553}, utility::Point{13.3868150, 49.7282850});
        result = CCalculateShortestRoute("czedges", utility::Point{15.568861848533453, 50.66747073900733}, utility::Point{ 15.581821585773252,  50.67989977147217});
    } catch (exception & e) {
        result = e.what();
    }
    std::cout << result;
}


*/