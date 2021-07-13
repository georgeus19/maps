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

class CsvConvertor {
public:

    CsvConvertor(const std::string& file) : f_(), file_(file) {}

    template <typename Graph>
    void SaveEdges(Graph& graph, const std::function<bool(const typename Graph::Edge&)> condition);

    template <typename Graph>
    void SaveVertexOrdering(Graph& graph);
private:
    std::ofstream f_;
    const std::string file_;

    template <typename Edge>
    void SaveEdge(const Edge& edge);

    template <typename Vertex>
    void SaveOrderingRank(const Vertex& vertex);

};

template <typename Graph>
void CsvConvertor::SaveEdges(Graph& graph, const std::function<bool(const typename Graph::Edge&)> condition) {
    try {
        f_.open(file_);
        graph.ForEachEdge([&](const typename Graph::Edge& edge) {
            if (condition(edge)) {
                SaveEdge<typename Graph::Edge>(edge);
            }

        });
    } catch (const std::exception& e) {
        std::cout << e.what();
        f_.close();
        throw;
    }

    f_.close();
}

template <typename Graph>
void CsvConvertor::SaveVertexOrdering(Graph& graph) {
    try {
        f_.open(file_);
        graph.ForEachVertex([&](typename Graph::Vertex& vertex) {
            SaveOrderingRank<typename Graph::Vertex&>(vertex);
        });
    } catch (const std::exception& e) {
        std::cout << e.what();
        f_.close();
        throw;
    }

    f_.close();
}


template<typename Edge>
void CsvConvertor::SaveEdge(const Edge& edge) {
    f_ << std::to_string(0) << ";"
        << std::to_string(edge.get_uid()) << ";" 
        << "null;" 
        << std::to_string(edge.get_from()) << ";" 
        << std::to_string(edge.get_to()) << ";" 
        << std::to_string(edge.IsTwoway()) << ";" 
        << std::to_string(edge.get_length()) << ";" 
        << std::to_string(edge.IsShortcut()) << ";"
        << std::to_string(edge.get_contracted_vertex());
    f_ << std::endl;
}



template <typename Vertex>
void CsvConvertor::SaveOrderingRank(const Vertex& vertex) {
    f_ << std::to_string(vertex.get_osm_id()) << ";"
        << std::to_string(vertex.get_ordering_rank());
    f_ << std::endl;
}




}
}
#endif //ROUTING_DATABASE_CSV_CONVERTOR_H