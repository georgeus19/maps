#ifndef BACKEND_VERTEX_H
#define BACKEND_VERTEX_H
#include "routing/edge.h"
#include <vector>
#include <limits>
namespace routing {

    /**
     * BasicVertex is a routing graph vertex with only the most general properties.
     */
    class BasicVertex {
    public:
        /**
         * Unique id.
         */
        unsigned_id_type osm_id_;

        /**
         * Edges which have origin in this to all neighbours.
         */
        std::vector<BasicEdge> outgoing_edges_;

        /**
         * How much it costs to go ti this vertex from the start point
         * of a routing algorithm.
         */
        double cost_;

        /**
         * Id of vertex that is one edge closer to the start
         * point of the best route.
         */
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
