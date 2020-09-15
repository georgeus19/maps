#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>

#include "routing/graph.h"
#include "routing/edge.h"
#include "routing/algorithm.h"
#include "routing/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "routing/basic_edge_endpoint_handler.h"
#include <string>

using namespace std;
using namespace routing;
using namespace database;

const string kDbName = "gis";
const string kUser = "postgres";
const string kPassword = "wtz2trln";
const string kHostAddress = "127.0.0.1";
const string kPort = "5432";
//void Test();

string CCalculateShortestRoute(const std::string & table_name, utility::Point start, utility::Point end) {
    if (start.lat_ == end.lat_ && start.lon_ == end.lon_) {
        throw RouteNotFoundException("Start and end point are the same.");
    }
    utility::Point graph_center{(start.lon_ + end.lon_) / 2, (start.lat_ + end.lat_) / 2};
    Dijkstra::G g{};
    DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    string radius = d.CalculateRadius(start, end, 0.7);
    d.LoadGraph<Dijkstra::G>(graph_center, radius, table_name, g);

    EndpointHandler<BasicEdgeEndpointHandler> start_handler{1, 1, 0, 0};
    auto && start_edges = start_handler.CalculateEndpointEdges(start, table_name, d);
    for(auto&& edge : start_edges) {
        auto e = edge;
        g.AddEdge(move(e));
    }

    unsigned_id_type free_node_id_from = start_handler.get_node_id_to();
    unsigned_id_type free_edge_id_from = start_handler.get_edge_id_to();
    EndpointHandler<BasicEdgeEndpointHandler> end_handler{free_node_id_from, free_node_id_from + 1, free_edge_id_from, free_edge_id_from + 4};
    auto && end_edges = end_handler.CalculateEndpointEdges(end, table_name, d);
    for(auto&& edge : end_edges) {
        auto e = edge;
        g.AddEdge(move(e));
    }

    Algorithm<Dijkstra> alg{g};
    vector<Dijkstra::Edge> res = alg.Run(0, 1);

    std::string geojson_array = d.GetRouteCoordinates(res, table_name);

    string first_edge_geometry = start_handler.GetEndpointEdgeGeometry(res[res.size() - 1].uid_);
    string last_edge_geometry = end_handler.GetEndpointEdgeGeometry(res[0].uid_);

    string final_array = "[" + first_edge_geometry + geojson_array + "," + last_edge_geometry + "]";
    return final_array;


/*
    auto && r_start = d.FindClosestEdge(start, table_name);
    BasicEdge start_edge{r_start};

    auto&& r_end = d.FindClosestEdge(end, table_name);
    BasicEdge end_edge{r_end};

    Algorithm<Dijkstra> alg{g};

    if (start_edge.get_to() != end_edge.get_to()) {
        vector<Dijkstra::Edge> res = alg.Run(start_edge.get_to(), end_edge.get_to());
        string route_coordinates = d.GetRouteCoordinates<Dijkstra::Edge>(res, table_name, start_edge, r_start, end_edge, r_end);
        return route_coordinates;
    } else { // rare case scenario - both points are on the same edge..
        vector<Dijkstra::Edge> res = alg.Run(start_edge.get_from(), end_edge.get_to());
        string route_coordinates = d.GetRouteCoordinates<Dijkstra::Edge>(res, table_name, start_edge, r_start, end_edge, r_end);
        return route_coordinates;
    }
    */
}
/**/
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
        result = CCalculateShortestRoute(table_name, utility::Point{start_lon, start_lat}, utility::Point{end_lon, end_lat});
        return Py_BuildValue("s", result.c_str());
    } catch (exception & e) {
        result = "ER" + string{e.what()};
        return Py_BuildValue("s", result.c_str());
    }


}

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
/*/
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

void Test() {
    Dijkstra::G g{};
    g.AddEdge(std::move(BasicEdge{0, 1, 2, 2}));
    g.AddEdge(std::move(BasicEdge{1, 1, 3, 2}));
    g.AddEdge(std::move(BasicEdge{2, 2, 6, 10}));
    g.AddEdge(std::move(BasicEdge{3, 3, 4, 3}));
    g.AddEdge(std::move(BasicEdge{4, 4, 3, 2}));
    g.AddEdge(std::move(BasicEdge{5, 4, 5, 2}));
    g.AddEdge(std::move(BasicEdge{6, 4, 6, 6}));
    g.AddEdge(std::move(BasicEdge{7, 5, 6, 2}));
    g.AddEdge(std::move(BasicEdge{8, 5, 3, 6}));

    Algorithm<Dijkstra> alg{g};
    auto && res = alg.Run(1, 6);
    for(auto&& e : res) {
        std::cout << e.from_ << "->" << e.to_ << std::endl;
    }
}
/**/