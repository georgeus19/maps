#ifndef BACKEND_VERTEX_H
#define BACKEND_VERTEX_H
#include "routing/edge.h"
#include <vector>
#include <limits>
namespace routing {

    template <typename T>
    class CostComparer {
    public:
        constexpr bool operator()( const T& p1, const T& p2 ) const {
            // Compare if these are same vertices.
            if (p1.second == p2.second) {
                return false;
            }
            // Compare costs.
            return  p1.first < p2.second;
        }
    };

    class BasicVertex {
    public:
        unsigned_id_type osm_id_;
        std::vector<BasicEdge> outgoing_edges_;
        double cost_;
        unsigned_id_type previous_;

        BasicVertex(unsigned_id_type osm_id, std::vector<BasicEdge> && outgoing_edges);

        BasicVertex(unsigned_id_type osm_id, const std::vector<BasicEdge> & outgoing_edges);

        BasicVertex(const BasicVertex & other);

        BasicVertex(BasicVertex && other);

        BasicVertex& operator=(const BasicVertex & other);

        BasicVertex& operator=(BasicVertex && other);

        ~BasicVertex();

        void Swap(BasicVertex & other);

        void AddEdge(BasicEdge&& edge);

        void AddEdge(const BasicEdge & edge);
    };
}
#endif //BACKEND_VERTEX_H
