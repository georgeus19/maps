#include "routing/graph.h"
#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>
#include "routing/edge.h"
#include "routing/algorithm.h"
#include "routing/dijkstra.h"
#include "database/database_helper.h"
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

string CCalculateShortestRoute(const std::string & table_name, Point start, Point end) {
    Point graph_center{(start.lon_ + end.lon_) / 2, (start.lat_ + end.lat_) / 2};
    Dijkstra::G g{};
    DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    string radius = d.CalculateRadius(start, end, 0.7);
    d.LoadGraph<Dijkstra::G>(graph_center, radius, table_name, g);
    auto && r = d.FindClosestEdge(start, table_name);
    BasicEdge start_edge{r};
    r = d.FindClosestEdge(end, table_name);
    BasicEdge end_edge{r};

    Algorithm<Dijkstra> alg{g};
    vector<Dijkstra::Edge> res = alg.Run(start_edge.get_from(), end_edge.get_to());
    string route_coordinates = d.GetRouteCoordinates<Dijkstra::Edge>(res, table_name);
    return route_coordinates;
}

static PyObject* CalculateShortestRoute(PyObject* self, PyObject* args) {
    const char * table_name;
    double start_lon;
    double start_lat;
    double end_lon;
    double end_lat;
    if (!PyArg_ParseTuple(args, "sdddd", &table_name, &start_lon, &start_lat, &end_lon, &end_lat)) {
        return NULL;
    }
    string result = CCalculateShortestRoute(table_name, Point{start_lon, start_lat}, Point{end_lon, end_lat});

    return Py_BuildValue("s", result.c_str());
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

/*
int main() {
    Test();
    Point start{13.3998825, 49.7230553};
    Point end{13.3868150, 49.7282850};
    string table_name = "cz_edges";
    std::cout << CalculateShortestRoute(table_name, start, end) << std::endl;
    Point graph_center{(start.lon_ + end.lon_) / 2, (start.lat_ + end.lat_) / 2};
    Dijkstra::G g{};
    DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    string radius = d.CalculateRadius(start, end, 0.7);
    d.LoadGraph<Dijkstra::G>(graph_center, radius, table_name, g);
    auto && r = d.FindClosestEdge(start, table_name);
    BasicEdge start_edge{r};
    r = d.FindClosestEdge(end, table_name);
    BasicEdge end_edge{r};

    Algorithm<Dijkstra> alg{g};
    vector<Dijkstra::Edge> res = alg.Run(start_edge.get_from(), end_edge.get_to());
    string route_coordinates = d.GetRouteCoordinates<Dijkstra::Edge>(res, table_name);
    std::cout << route_coordinates << std::endl;
    for(auto&& e : res) {
        std::cout << e.from_ << "->" << e.to_ << std::endl;
    }
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
 */