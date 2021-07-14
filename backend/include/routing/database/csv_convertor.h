#ifndef ROUTING_DATABASE_CSV_CONVERTOR_H
#define ROUTING_DATABASE_CSV_CONVERTOR_H

#include "routing/types.h"

#include <vector>
#include <set>
#include <queue>
#include <iostream>
#include <fstream>
#include <cassert>
#include <functional>
#include <string_view>

namespace routing {
namespace database {

template <typename Edge>
class CHEdgeConvertor{
public:

    CHEdgeConvertor() {}

    void SaveEdge(std::ofstream& f, const Edge& edge) const {
        f   << std::to_string(0) << ";"
            << std::to_string(edge.get_uid()) << ";" 
            << "null;" 
            << std::to_string(edge.get_from()) << ";" 
            << std::to_string(edge.get_to()) << ";" 
            << std::to_string(edge.IsTwoway()) << ";" 
            << std::to_string(edge.get_length()) << ";" 
            << std::to_string(edge.IsShortcut()) << ";"
            << std::to_string(edge.get_contracted_vertex());
        f << std::endl;
    }
};

template <typename Vertex>
class CHVertexConvertor{
public:

    CHVertexConvertor() {}

    void SaveVertex(std::ofstream& f, const Vertex& vertex) const {
        f   << std::to_string(vertex.get_uid()) << ";"
            << std::to_string(vertex.get_ordering_rank());
        f << std::endl;
    }
};

class CsvConvertor {
public:

    CsvConvertor(const std::string& file) : f_(), file_(file) {}

    template <typename Graph, typename EdgeConvertor>
    void SaveEdges(Graph& graph, const EdgeConvertor& edge_convertor, const std::function<bool(const typename Graph::Edge&)> condition);

    template <typename Graph, typename VertexConvertor>
    void SaveVertices(Graph& graph, const VertexConvertor& vertex_convertor);
private:
    std::ofstream f_;
    const std::string file_;
};

template <typename Graph, typename EdgeConvertor>
void CsvConvertor::SaveEdges(Graph& graph, const EdgeConvertor& edge_convertor, const std::function<bool(const typename Graph::Edge&)> condition) {
    try {
        f_.open(file_);
        graph.ForEachEdge([&](const typename Graph::Edge& edge) {
            if (condition(edge)) {
                edge_convertor.SaveEdge(f_, edge);
            }

        });
    } catch (const std::exception& e) {
        std::cout << e.what();
        f_.close();
        throw;
    }

    f_.close();
}

template <typename Graph, typename VertexConvertor>
void CsvConvertor::SaveVertices(Graph& graph, const VertexConvertor& vertex_convertor) {
    try {
        f_.open(file_);
        graph.ForEachVertex([&](typename Graph::Vertex& vertex) {
            vertex_convertor.SaveVertex(f_, vertex);
        });
    } catch (const std::exception& e) {
        std::cout << e.what();
        f_.close();
        throw;
    }

    f_.close();
}





}
}
#endif //ROUTING_DATABASE_CSV_CONVERTOR_H