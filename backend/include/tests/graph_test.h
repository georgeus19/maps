#ifndef GRAPH_TEST_H
#define GRAPH_TEST_H

#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/query/dijkstra.h"
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
        graph.AddEdge(std::move(typename G::Edge{0, 1, 2, 2}));
        graph.AddEdge(std::move(typename G::Edge{1, 1, 3, 2}));
        graph.AddEdge(std::move(typename G::Edge{2, 2, 6, 8}));
        graph.AddEdge(std::move(typename G::Edge{3, 3, 4, 3}));
        graph.AddEdge(std::move(typename G::Edge{4, 4, 3, 2}));
        graph.AddEdge(std::move(typename G::Edge{5, 4, 5, 2}));
        graph.AddEdge(std::move(typename G::Edge{6, 5, 4, 4}));
        graph.AddEdge(std::move(typename G::Edge{7, 4, 6, 6}));
        graph.AddEdge(std::move(typename G::Edge{8, 5, 6, 2}));
        graph.AddEdge(std::move(typename G::Edge{9, 6, 5, 3}));
        graph.AddEdge(std::move(typename G::Edge{10, 5, 3, 7}));
}

template <typename G>
void TestBasicContractedGraph(G & graph, bool with_shortcuts = true) {
        graph.AddEdge(std::move(typename G::Edge{0, 1, 2, 2}));
        graph.AddEdge(std::move(typename G::Edge{1, 1, 3, 2}));
        if (with_shortcuts) {
                graph.AddEdge(std::move(typename G::Edge{2, 1, 4, 5, 3}));
        } else {
                graph.AddEdge(std::move(typename G::Edge{2, 1, 4, 5}));
        }
        
        
        graph.AddEdge(std::move(typename G::Edge{3, 2, 6, 8}));
        if (with_shortcuts) {
                graph.AddEdge(std::move(typename G::Edge{4, 2, 5, 11, 6}));
        } else {
                graph.AddEdge(std::move(typename G::Edge{4, 2, 5, 11}));
        }
        if (with_shortcuts) {
                graph.AddEdge(std::move(typename G::Edge{5, 2, 4, 15, 5}));
        } else {
                graph.AddEdge(std::move(typename G::Edge{5, 2, 4, 15}));
        }
        
        
        graph.AddEdge(std::move(typename G::Edge{6, 3, 4, 3}));
        
        graph.AddEdge(std::move(typename G::Edge{7, 4, 3, 2}));
        graph.AddEdge(std::move(typename G::Edge{8, 4, 5, 2}));
        graph.AddEdge(std::move(typename G::Edge{9, 4, 6, 6}));
        
        graph.AddEdge(std::move(typename G::Edge{10, 5, 4, 4}));
        graph.AddEdge(std::move(typename G::Edge{11, 5, 3, 7}));
        graph.AddEdge(std::move(typename G::Edge{12, 5, 6, 2}));

        graph.AddEdge(std::move(typename G::Edge{13, 6, 5, 3}));

        graph.GetVertex(1).set_ordering_rank(3);
        graph.GetVertex(2).set_ordering_rank(5);
        graph.GetVertex(3).set_ordering_rank(1);
        graph.GetVertex(4).set_ordering_rank(6);
        graph.GetVertex(5).set_ordering_rank(4);
        graph.GetVertex(6).set_ordering_rank(2);

}

template <typename G>
void TestPathGraph(G& g) {
    g.AddEdge(std::move(typename G::Edge{0, 1, 2, 1}));
 
    g.AddEdge(std::move(typename G::Edge{1, 2, 3, 2}));

    g.AddEdge(std::move(typename G::Edge{2, 3, 4, 3}));

    g.AddEdge(std::move(typename G::Edge{3, 4, 5, 4}));
    
    g.AddEdge(std::move(typename G::Edge{4, 5, 6, 5}));

    g.AddEdge(std::move(typename G::Edge{5, 6, 7, 4}));

    g.AddEdge(std::move(typename G::Edge{6, 7, 8, 3}));

    g.AddEdge(std::move(typename G::Edge{7, 8, 9, 2}));

    g.AddEdge(std::move(typename G::Edge{77, 9, 10, 1}));

    g.GetVertex(1).set_ordering_rank(1);
    g.GetVertex(2).set_ordering_rank(2);
    g.GetVertex(3).set_ordering_rank(3);
    g.GetVertex(4).set_ordering_rank(4);
    g.GetVertex(5).set_ordering_rank(5);
    g.GetVertex(6).set_ordering_rank(10);
    g.GetVertex(7).set_ordering_rank(9);
    g.GetVertex(8).set_ordering_rank(8);
    g.GetVertex(9).set_ordering_rank(7);
    g.GetVertex(10).set_ordering_rank(6);
}

template <typename G>
void TestPathShortcutGraph(G& g) {
    TestPathGraph(g);
    g.AddEdge(std::move(typename G::Edge{8, 1, 3, 3, 2}));
    g.AddEdge(std::move(typename G::Edge{9, 1, 4, 6, 3}));
    g.AddEdge(std::move(typename G::Edge{10, 1, 5, 10, 4}));
 
    g.AddEdge(std::move(typename G::Edge{11, 5, 10, 15, 6}));

    g.AddEdge(std::move(typename G::Edge{12, 6, 10, 10, 7}));

    g.AddEdge(std::move(typename G::Edge{13, 7, 10, 6, 8}));

    g.AddEdge(std::move(typename G::Edge{14, 8, 10, 3, 9}));

    g.GetVertex(1).set_ordering_rank(9);    
    g.GetVertex(2).set_ordering_rank(1);
    g.GetVertex(3).set_ordering_rank(2);
    g.GetVertex(4).set_ordering_rank(3);
    g.GetVertex(5).set_ordering_rank(10);
    g.GetVertex(6).set_ordering_rank(7);
    g.GetVertex(7).set_ordering_rank(6);
    g.GetVertex(8).set_ordering_rank(5);
    g.GetVertex(9).set_ordering_rank(4);
    g.GetVertex(10).set_ordering_rank(8);
}


template <typename G>
void TestBidirectedGraph(G& graph) {
	graph.AddEdge(std::move(typename G::Edge{0, 1, 2, 2, G::Edge::EdgeType::forward}));
	graph.AddEdge(std::move(typename G::Edge{1, 1, 3, 3, G::Edge::EdgeType::twoway}));
	graph.AddEdge(std::move(typename G::Edge{2, 2, 6, 12, G::Edge::EdgeType::forward}));
	graph.AddEdge(std::move(typename G::Edge{3, 3, 4, 2, G::Edge::EdgeType::twoway}));
	graph.AddEdge(std::move(typename G::Edge{4, 4, 5, 3, G::Edge::EdgeType::twoway}));
	graph.AddEdge(std::move(typename G::Edge{5, 4, 6, 6, G::Edge::EdgeType::forward}));
	graph.AddEdge(std::move(typename G::Edge{6, 5, 6, 2, G::Edge::EdgeType::forward}));
	graph.AddEdge(std::move(typename G::Edge{7, 6, 5, 3, G::Edge::EdgeType::forward}));
}

template <typename G>
void TestBidirectedSearchGraph(G& graph) {
	graph.AddEdge(std::move(typename G::Edge{0, 1, 2, 2, G::Edge::EdgeType::forward}));
	graph.AddEdge(std::move(typename G::Edge{1, 1, 3, 3, G::Edge::EdgeType::twoway}));
	graph.AddEdge(std::move(typename G::Edge{2, 2, 6, 12, G::Edge::EdgeType::forward}));
	graph.AddEdge(std::move(typename G::Edge{3, 3, 4, 2, G::Edge::EdgeType::twoway}));
	graph.AddEdge(std::move(typename G::Edge{4, 4, 5, 3, G::Edge::EdgeType::twoway}));
	graph.AddEdge(std::move(typename G::Edge{5, 4, 6, 6, G::Edge::EdgeType::forward}));
	graph.AddEdge(std::move(typename G::Edge{6, 5, 6, 2, G::Edge::EdgeType::forward}));
	graph.AddEdge(std::move(typename G::Edge{7, 6, 5, 3, G::Edge::EdgeType::forward}));

	// Shortcuts.
	graph.AddEdge(std::move(typename G::Edge{8, 1, 4, 3, G::Edge::EdgeType::twoway, 3}));
	graph.AddEdge(std::move(typename G::Edge{9, 1, 5, 8, G::Edge::EdgeType::twoway, 4}));
	graph.AddEdge(std::move(typename G::Edge{10, 1, 6, 11, G::Edge::EdgeType::forward, 4}));
	graph.AddEdge(std::move(typename G::Edge{11, 5, 2, 10, G::Edge::EdgeType::forward, 1}));
	graph.AddEdge(std::move(typename G::Edge{12, 2, 5, 15, G::Edge::EdgeType::forward, 6}));

	graph.GetVertex(1).set_ordering_rank(3);   
	graph.GetVertex(2).set_ordering_rank(5);   
	graph.GetVertex(3).set_ordering_rank(1);   
	graph.GetVertex(4).set_ordering_rank(2);   
	graph.GetVertex(5).set_ordering_rank(6);   
	graph.GetVertex(6).set_ordering_rank(4);   
}


#endif // GRAPH_TEST_H
