#ifndef BACKEND_CONTRACTION_VERTEX_H
#define BACKEND_CONTRACTION_VERTEX_H
#include "routing/edge.h"
#include "routing/vertices/basic_vertex.h"
#include <vector>
#include <limits>
namespace routing {
    template <typename Edge>
    class ContractionVertex : public BasicVertex<Edge> {
    private:

        unsigned_id_type order_id_;

        bool contracted_;

        std::vector<Edge> reverse_edges_;
    public:

        ContractionVertex(unsigned_id_type osm_id);

        void AddReverseEdge(Edge&& edge);

        void AddReverseEdge(const Edge & edge);
    };

    template <typename Edge>
    ContractionVertex<Edge>::ContractionVertex(unsigned_id_type osm_id)
            : BasicVertex<Edge>(osm_id), contracted_(false), reverse_edges_(std::vector<Edge>{}) {}

    template <typename Edge>
    void ContractionVertex<Edge>::AddReverseEdge(Edge&& edge) {
        reverse_edges_.push_back(std::move(edge));
    }

    template <typename Edge>
    void ContractionVertex<Edge>::AddReverseEdge(const Edge & edge) {
        reverse_edges_.push_back(edge);
    }

}

#endif //BACKEND_CONTRACTION_VERTEX_H
