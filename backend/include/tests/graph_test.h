#ifndef GRAPH_TEST_H
#define GRAPH_TEST_H

#include "routing/graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "routing/basic_edge_endpoint_handler.h"

template <typename G>
void TestBasicGraph(G & graph) {
        graph.AddEdge(std::move(routing::BasicEdge{0, 1, 2, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{1, 1, 3, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{2, 2, 6, 8}));
        graph.AddEdge(std::move(routing::BasicEdge{3, 3, 4, 3}));
        graph.AddEdge(std::move(routing::BasicEdge{4, 4, 3, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{5, 4, 5, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{6, 5, 4, 4}));
        graph.AddEdge(std::move(routing::BasicEdge{7, 4, 6, 6}));
        graph.AddEdge(std::move(routing::BasicEdge{8, 5, 6, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{9, 6, 5, 3}));
        graph.AddEdge(std::move(routing::BasicEdge{10, 5, 3, 7}));
}

template <typename G>
void TestBasicReverseGraph(G & graph) {
        graph.AddEdge(std::move(routing::BasicEdge{0, 1, 2, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{0, 1, 2, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{1, 1, 3, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{1, 1, 3, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{2, 2, 6, 8}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{2, 2, 6, 8}));
        graph.AddEdge(std::move(routing::BasicEdge{3, 3, 4, 3}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{3, 3, 4, 3}));
        graph.AddEdge(std::move(routing::BasicEdge{4, 4, 3, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{4, 4, 3, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{5, 4, 5, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{5, 4, 5, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{6, 5, 4, 4}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{6, 5, 4, 4}));
        graph.AddEdge(std::move(routing::BasicEdge{7, 4, 6, 6}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{7, 4, 6, 6}));
        graph.AddEdge(std::move(routing::BasicEdge{8, 5, 6, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{8, 5, 6, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{9, 6, 5, 3}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{9, 6, 5, 3}));
        graph.AddEdge(std::move(routing::BasicEdge{10, 5, 3, 7}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{10, 5, 3, 7}));
}

template <typename G>
void TestBasicContractedGraph(G & graph) {
        graph.AddEdge(std::move(routing::BasicEdge{0, 1, 2, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{0, 1, 2, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{1, 1, 3, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{1, 1, 3, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{2, 1, 4, 5}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{2, 1, 4, 5}));
        
        graph.AddEdge(std::move(routing::BasicEdge{3, 2, 6, 8}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{3, 2, 6, 8}));
        graph.AddEdge(std::move(routing::BasicEdge{4, 2, 5, 11}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{4, 2, 5, 11}));
        graph.AddEdge(std::move(routing::BasicEdge{5, 2, 4, 15}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{5, 2, 4, 15}));
        
        graph.AddEdge(std::move(routing::BasicEdge{6, 3, 4, 3}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{6, 3, 4, 3}));
        
        graph.AddEdge(std::move(routing::BasicEdge{7, 4, 3, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{7, 4, 3, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{8, 4, 5, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{8, 4, 5, 2}));
        graph.AddEdge(std::move(routing::BasicEdge{9, 4, 6, 6}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{9, 4, 6, 6}));
        
        graph.AddEdge(std::move(routing::BasicEdge{10, 5, 4, 4}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{10, 5, 4, 4}));
        graph.AddEdge(std::move(routing::BasicEdge{11, 5, 3, 7}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{11, 5, 3, 7}));
        graph.AddEdge(std::move(routing::BasicEdge{12, 5, 6, 2}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{12, 5, 6, 2}));

        graph.AddEdge(std::move(routing::BasicEdge{13, 6, 5, 3}));
        graph.AddReverseEdge(std::move(routing::BasicEdge{13, 6, 5, 3}));

}


#endif // GRAPH_TEST_H
