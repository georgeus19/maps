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
#include "routing/edges/ch_preprocessing_edge.h"
#include "routing/edges/ch_search_edge.h"

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
        graph.AddEdge(std::move(typename G::E{0, 1, 2, 2}));
        graph.AddReverseEdge(std::move(typename G::E{0, 1, 2, 2}));
        graph.AddEdge(std::move(typename G::E{1, 1, 3, 2}));
        graph.AddReverseEdge(std::move(typename G::E{1, 1, 3, 2}));
        graph.AddEdge(std::move(typename G::E{2, 2, 6, 8}));
        graph.AddReverseEdge(std::move(typename G::E{2, 2, 6, 8}));
        graph.AddEdge(std::move(typename G::E{3, 3, 4, 3}));
        graph.AddReverseEdge(std::move(typename G::E{3, 3, 4, 3}));
        graph.AddEdge(std::move(typename G::E{4, 4, 3, 2}));
        graph.AddReverseEdge(std::move(typename G::E{4, 4, 3, 2}));
        graph.AddEdge(std::move(typename G::E{5, 4, 5, 2}));
        graph.AddReverseEdge(std::move(typename G::E{5, 4, 5, 2}));
        graph.AddEdge(std::move(typename G::E{6, 5, 4, 4}));
        graph.AddReverseEdge(std::move(typename G::E{6, 5, 4, 4}));
        graph.AddEdge(std::move(typename G::E{7, 4, 6, 6}));
        graph.AddReverseEdge(std::move(typename G::E{7, 4, 6, 6}));
        graph.AddEdge(std::move(typename G::E{8, 5, 6, 2}));
        graph.AddReverseEdge(std::move(typename G::E{8, 5, 6, 2}));
        graph.AddEdge(std::move(typename G::E{9, 6, 5, 3}));
        graph.AddReverseEdge(std::move(typename G::E{9, 6, 5, 3}));
        graph.AddEdge(std::move(typename G::E{10, 5, 3, 7}));
        graph.AddReverseEdge(std::move(typename G::E{10, 5, 3, 7}));
}

template <typename G>
void TestBasicContractedGraph(G & graph, bool with_shortcuts = true) {
        graph.AddEdge(std::move(typename G::E{0, 1, 2, 2}));
        graph.AddReverseEdge(std::move(typename G::E{0, 1, 2, 2}));
        graph.AddEdge(std::move(typename G::E{1, 1, 3, 2}));
        graph.AddReverseEdge(std::move(typename G::E{1, 1, 3, 2}));
        if (with_shortcuts) {
                graph.AddEdge(std::move(typename G::E{2, 1, 4, 5, 3}));
                graph.AddReverseEdge(std::move(typename G::E{2, 1, 4, 5, 3}));
        } else {
                graph.AddEdge(std::move(typename G::E{2, 1, 4, 5}));
                graph.AddReverseEdge(std::move(typename G::E{2, 1, 4, 5}));
        }
        
        
        graph.AddEdge(std::move(typename G::E{3, 2, 6, 8}));
        graph.AddReverseEdge(std::move(typename G::E{3, 2, 6, 8}));
        if (with_shortcuts) {
                graph.AddEdge(std::move(typename G::E{4, 2, 5, 11, 6}));
                graph.AddReverseEdge(std::move(typename G::E{4, 2, 5, 11, 6}));
        } else {
                graph.AddEdge(std::move(typename G::E{4, 2, 5, 11}));
                graph.AddReverseEdge(std::move(typename G::E{4, 2, 5, 11}));
        }
        if (with_shortcuts) {
                graph.AddEdge(std::move(typename G::E{5, 2, 4, 15, 5}));
                graph.AddReverseEdge(std::move(typename G::E{5, 2, 4, 15, 5}));
        } else {
                graph.AddEdge(std::move(typename G::E{5, 2, 4, 15}));
                graph.AddReverseEdge(std::move(typename G::E{5, 2, 4, 15}));
        }
        
        
        graph.AddEdge(std::move(typename G::E{6, 3, 4, 3}));
        graph.AddReverseEdge(std::move(typename G::E{6, 3, 4, 3}));
        
        graph.AddEdge(std::move(typename G::E{7, 4, 3, 2}));
        graph.AddReverseEdge(std::move(typename G::E{7, 4, 3, 2}));
        graph.AddEdge(std::move(typename G::E{8, 4, 5, 2}));
        graph.AddReverseEdge(std::move(typename G::E{8, 4, 5, 2}));
        graph.AddEdge(std::move(typename G::E{9, 4, 6, 6}));
        graph.AddReverseEdge(std::move(typename G::E{9, 4, 6, 6}));
        
        graph.AddEdge(std::move(typename G::E{10, 5, 4, 4}));
        graph.AddReverseEdge(std::move(typename G::E{10, 5, 4, 4}));
        graph.AddEdge(std::move(typename G::E{11, 5, 3, 7}));
        graph.AddReverseEdge(std::move(typename G::E{11, 5, 3, 7}));
        graph.AddEdge(std::move(typename G::E{12, 5, 6, 2}));
        graph.AddReverseEdge(std::move(typename G::E{12, 5, 6, 2}));

        graph.AddEdge(std::move(typename G::E{13, 6, 5, 3}));
        graph.AddReverseEdge(std::move(typename G::E{13, 6, 5, 3}));

        graph.GetVertex(1).set_ordering_rank(3);
        graph.GetVertex(2).set_ordering_rank(5);
        graph.GetVertex(3).set_ordering_rank(1);
        graph.GetVertex(4).set_ordering_rank(6);
        graph.GetVertex(5).set_ordering_rank(4);
        graph.GetVertex(6).set_ordering_rank(2);

}

template <typename G>
void TestPathGraph(G& g) {
    g.AddEdge(std::move(typename G::E{0, 1, 2, 1}));
    g.AddReverseEdge(std::move(typename G::E{0, 1, 2, 1}));
 
    g.AddEdge(std::move(typename G::E{1, 2, 3, 2}));
    g.AddReverseEdge(std::move(typename G::E{1, 2, 3, 2}));

    g.AddEdge(std::move(typename G::E{2, 3, 4, 3}));
    g.AddReverseEdge(std::move(typename G::E{2, 3, 4, 3}));

    g.AddEdge(std::move(typename G::E{3, 4, 5, 4}));
    g.AddReverseEdge(std::move(typename G::E{3, 4, 5, 4}));
    
    g.AddEdge(std::move(typename G::E{4, 5, 6, 5}));
    g.AddReverseEdge(std::move(typename G::E{4, 5, 6, 5}));

    g.AddEdge(std::move(typename G::E{5, 6, 7, 4}));
    g.AddReverseEdge(std::move(typename G::E{5, 6, 7, 4}));

    g.AddEdge(std::move(typename G::E{6, 7, 8, 3}));
    g.AddReverseEdge(std::move(typename G::E{6, 7, 8, 3}));

    g.AddEdge(std::move(typename G::E{7, 8, 9, 2}));
    g.AddReverseEdge(std::move(typename G::E{7, 8, 9, 2}));

    g.AddEdge(std::move(typename G::E{77, 9, 10, 1}));
    g.AddReverseEdge(std::move(typename G::E{77, 9, 10, 1}));

    g.GetVertex(1).set_ordering_rank(1);
    g.GetVertex(2).set_ordering_rank(2);
    g.GetVertex(3).set_ordering_rank(3);
    g.GetVertex(4).set_ordering_rank(4);
    g.GetVertex(5).set_ordering_rank(10);
    g.GetVertex(6).set_ordering_rank(9);
    g.GetVertex(7).set_ordering_rank(8);
    g.GetVertex(8).set_ordering_rank(7);
    g.GetVertex(9).set_ordering_rank(6);
    g.GetVertex(10).set_ordering_rank(5);
}

template <typename G>
void TestPathShortcutGraph(G& g) {
    TestPathGraph(g);
    g.AddEdge(std::move(typename G::E{8, 1, 3, 3, 2}));
    g.AddReverseEdge(std::move(typename G::E{8, 1, 3, 3, 2}));
    g.AddEdge(std::move(typename G::E{9, 1, 4, 6, 3}));
    g.AddReverseEdge(std::move(typename G::E{9, 1, 4, 6, 3}));
    g.AddEdge(std::move(typename G::E{10, 1, 5, 10, 4}));
    g.AddReverseEdge(std::move(typename G::E{10, 1, 5, 10, 4}));
 
    g.AddEdge(std::move(typename G::E{11, 5, 10, 15, 6}));
    g.AddReverseEdge(std::move(typename G::E{11, 5, 10, 15, 6}));

    g.AddEdge(std::move(typename G::E{12, 6, 10, 10, 7}));
    g.AddReverseEdge(std::move(typename G::E{12, 6, 10, 10, 7}));

    g.AddEdge(std::move(typename G::E{13, 7, 10, 6, 8}));
    g.AddReverseEdge(std::move(typename G::E{13, 7, 10, 6, 8}));

    g.AddEdge(std::move(typename G::E{14, 8, 10, 3, 9}));
    g.AddReverseEdge(std::move(typename G::E{14, 8, 10, 3, 9}));
}


#endif // GRAPH_TEST_H
