#ifndef BACKEND_VERTEX_H
#define BACKEND_VERTEX_H
#include "routing/edge.h"
#include <vector>
#include <limits>
namespace routing {

    /**
     * BasicVertex is a routing graph vertex with only the most general properties.
     */
    template <typename Edge>
    class BasicVertex {
    public:
        /**
         * Unique id.
         */
        unsigned_id_type osm_id_;

        /**
         * Edges which have origin in this to all neighbours.
         */
        std::vector<Edge> outgoing_edges_;

        /**
         * How much it costs to go to this vertex from the start point
         * of a routing algorithm.
         */
        double cost_;

        /**
         * Id of vertex that is one edge closer to the start
         * point of the best route.
         */
        unsigned_id_type previous_;

        BasicVertex(unsigned_id_type osm_id, std::vector<Edge> && outgoing_edges);

        BasicVertex(unsigned_id_type osm_id, const std::vector<Edge> & outgoing_edges);

        BasicVertex(const BasicVertex<Edge> & other);

        BasicVertex(BasicVertex<Edge> && other);

        BasicVertex& operator=(const BasicVertex & other);

        BasicVertex& operator=(BasicVertex && other);

        ~BasicVertex();

        void Swap(BasicVertex & other);

        virtual void AddEdge(Edge&& edge);

        virtual void AddEdge(const Edge & edge);

        unsigned_id_type GetPreviousDefaultValue() const;

        void ResetRoutingProperties();
    };

    template <typename Edge>
    class ContractionVertex : public BasicVertex<Edge> {
    public:

        unsigned_id_type order_id_;

        bool contracted_;

        std::vector<Edge*> ingoing_edges_;

        ContractionVertex(unsigned_id_type osm_id, std::vector<Edge> && outgoing_edges);

        ContractionVertex(unsigned_id_type osm_id, const std::vector<Edge> & outgoing_edges);

        void AddEdge(Edge&& edge);

        void AddEdge(const Edge & edge);
    };

    template <typename Edge>
    BasicVertex<Edge>::BasicVertex(unsigned_id_type osm_id, std::vector<Edge> && outgoing_edges)
            : osm_id_(osm_id), outgoing_edges_(std::move(outgoing_edges)), cost_(std::numeric_limits<double>::max()), previous_(GetPreviousDefaultValue()) {}

    template <typename Edge>
    BasicVertex<Edge>::BasicVertex(unsigned_id_type osm_id, const std::vector<Edge> & outgoing_edges)
            : osm_id_(osm_id), outgoing_edges_(outgoing_edges), cost_(std::numeric_limits<double>::max()), previous_(GetPreviousDefaultValue()) {}

    template <typename Edge>
    BasicVertex<Edge>::BasicVertex(const BasicVertex<Edge> & other) {
        osm_id_ = other.osm_id_;
        outgoing_edges_ = other.outgoing_edges_;
        cost_ = other.cost_;
        previous_ = other.previous_;
    }

    template <typename Edge>
    BasicVertex<Edge>::BasicVertex(BasicVertex<Edge> && other) {
        osm_id_ = other.osm_id_;
        outgoing_edges_ = std::move(other.outgoing_edges_);
        other.outgoing_edges_ = std::vector<Edge>{};
        cost_ = other.cost_;
        previous_ = other.previous_;
    }

    template <typename Edge>
    BasicVertex<Edge>& BasicVertex<Edge>::operator=(const BasicVertex<Edge> & other) {
        BasicVertex tmp{other};
        Swap(tmp);
        return *this;
    }

    template <typename Edge>
    BasicVertex<Edge>& BasicVertex<Edge>::operator=(BasicVertex<Edge> && other) {
        if (this != &other) {
            osm_id_ = other.osm_id_;
            outgoing_edges_ = std::move(other.outgoing_edges_);
            other.outgoing_edges_ = std::vector<Edge>{};
            cost_ = other.cost_;
            previous_ = other.previous_;
        }
        return *this;
    }

    template <typename Edge>
    BasicVertex<Edge>::~BasicVertex() {}

    template <typename Edge>
    void BasicVertex<Edge>::Swap(BasicVertex<Edge> & other) {
        std::swap(osm_id_, other.osm_id_);
        std::swap(outgoing_edges_, other.outgoing_edges_);
        std::swap(cost_, other.cost_);
        std::swap(previous_, other.previous_);
    }

    template <typename Edge>
    void BasicVertex<Edge>::AddEdge(Edge&& edge) {
        outgoing_edges_.push_back(std::move(edge));
    }

    template <typename Edge>
    void BasicVertex<Edge>::AddEdge(const Edge & edge) {
        outgoing_edges_.push_back(edge);
    }

    template <typename Edge>
    unsigned_id_type BasicVertex<Edge>::GetPreviousDefaultValue() const {
        return std::numeric_limits<unsigned_id_type>::max();
    }

    template <typename Edge>
    void BasicVertex<Edge>::ResetRoutingProperties() {
        previous_ = GetPreviousDefaultValue();
        cost_ = std::numeric_limits<double>::max();
    }

    template <typename Edge>
    ContractionVertex<Edge>::ContractionVertex(unsigned_id_type osm_id, std::vector<Edge> && outgoing_edges)
            : BasicVertex<Edge>(osm_id, std::move(outgoing_edges)), contracted_(false) {
        ingoing_edges_ = std::vector<Edge*>{};
        std::for_each(BasicVertex<Edge>::outgoing_edges_.begin(), BasicVertex<Edge>::outgoing_edges_.end(), [&](Edge & edge) {
            ingoing_edges_.push_back(&edge);
        });
    }

    template <typename Edge>
    ContractionVertex<Edge>::ContractionVertex(unsigned_id_type osm_id, const std::vector<Edge> & outgoing_edges)
            : BasicVertex<Edge>(osm_id, outgoing_edges), contracted_(false) {
        ingoing_edges_ = std::vector<Edge*>{};
        std::for_each(BasicVertex<Edge>::outgoing_edges_.begin(), BasicVertex<Edge>::outgoing_edges_.end(), [&](Edge & edge) {
            ingoing_edges_.push_back(&edge);
        });
    }

    template <typename Edge>
    void ContractionVertex<Edge>::AddEdge(Edge&& edge) {
        BasicVertex<Edge>::outgoing_edges_.push_back(std::move(edge));
        ingoing_edges_.push_back(&BasicVertex<Edge>::outgoing_edges_.back());
    }

    template <typename Edge>
    void ContractionVertex<Edge>::AddEdge(const Edge & edge) {
        BasicVertex<Edge>::outgoing_edges_.push_back(edge);
        ingoing_edges_.push_back(&BasicVertex<Edge>::outgoing_edges_.back());
    }

}

#endif //BACKEND_VERTEX_H
