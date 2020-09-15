#include "routing/vertex.h"

using namespace std;

namespace routing {
    
    BasicVertex::BasicVertex(unsigned_id_type osm_id, std::vector<BasicEdge> && outgoing_edges)
        : osm_id_(osm_id), outgoing_edges_(std::move(outgoing_edges)), cost_(numeric_limits<double>::max()), previous_(numeric_limits<unsigned_id_type>::max()) {}

    BasicVertex::BasicVertex(unsigned_id_type osm_id, const std::vector<BasicEdge> & outgoing_edges)
            : osm_id_(osm_id), outgoing_edges_(outgoing_edges), cost_(numeric_limits<double>::max()), previous_(numeric_limits<unsigned_id_type>::max()) {}

    BasicVertex::BasicVertex(const BasicVertex & other) {
        osm_id_ = other.osm_id_;
        outgoing_edges_ = other.outgoing_edges_;
        cost_ = other.cost_;
        previous_ = other.previous_;
    }

    BasicVertex::BasicVertex(BasicVertex && other) {
        osm_id_ = other.osm_id_;
        outgoing_edges_ = std::move(other.outgoing_edges_);
        other.outgoing_edges_ = vector<BasicEdge>{};
        cost_ = other.cost_;
        previous_ = other.previous_;
    }

    BasicVertex& BasicVertex::operator=(const BasicVertex & other) {
        BasicVertex tmp{other};
        Swap(tmp);
        return *this;
    }

    BasicVertex& BasicVertex::operator=(BasicVertex && other) {
        if (this != &other) {
            osm_id_ = other.osm_id_;
            outgoing_edges_ = std::move(other.outgoing_edges_);
            other.outgoing_edges_ = vector<BasicEdge>{};
            cost_ = other.cost_;
            previous_ = other.previous_;
        }
        return *this;
    }

    BasicVertex::~BasicVertex() {}

    void BasicVertex::Swap(BasicVertex & other) {
        std::swap(osm_id_, other.osm_id_);
        std::swap(outgoing_edges_, other.outgoing_edges_);
        std::swap(cost_, other.cost_);
        std::swap(previous_, other.previous_);
    }

    void BasicVertex::AddEdge(BasicEdge&& edge) {
        outgoing_edges_.push_back(std::move(edge));
    }


    void BasicVertex::AddEdge(const BasicEdge & edge) {
        outgoing_edges_.push_back(edge);
    }
}