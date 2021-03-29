#ifndef BACKEND_CH_DIJKSTRA_H
#define BACKEND_CH_DIJKSTRA_H
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <cassert>
#include <unordered_map>

#include "routing/query/route_retriever.h"
#include "routing/exception.h"
#include "routing/edges/basic_edge.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/graph.h"

namespace routing {
namespace preprocessing {
/**
 * Implementation of dijkstra's routing algorithm.
 */
template <typename G>
class CHDijkstra {
public:
    using Vertex = typename G::V;
    using Edge = typename G::E;
    using Graph = G;

    struct SearchRangeLimits {
        double max_cost;
        size_t max_hop_count;
		
		SearchRangeLimits(double c, size_t h) : max_cost(c), max_hop_count(h) {}

    };

    CHDijkstra(G & g);

    bool Run(unsigned_id_type source_vertex, unsigned_id_type contracted_vertex, const SearchRangeLimits& limits);

    double GetPathLength(unsigned_id_type to) const;

	double OneHopBackwardSearch(unsigned_id_type target_vertex_id) const;
private:
    struct VertexRoutingInfo;
	struct PriorityQueueMember;
	struct PriorityQueueMemberMinComparator;
    using PriorityQueue = std::priority_queue<PriorityQueueMember, std::vector<PriorityQueueMember>, PriorityQueueMemberMinComparator>;
    G& g_;

    std::unordered_map<unsigned_id_type, VertexRoutingInfo> touched_vertices_;

    unsigned_id_type source_vertex_;
	unsigned_id_type contracted_vertex_;

    struct VertexRoutingInfo {
        double cost;
        unsigned_id_type previous;

		VertexRoutingInfo() : cost(std::numeric_limits<double>::max()), previous(0) {}

        VertexRoutingInfo(double c, unsigned_id_type p) : cost(c), previous(p) {}

        VertexRoutingInfo(const VertexRoutingInfo& other) = default;
        VertexRoutingInfo(VertexRoutingInfo&& other) = default;
        VertexRoutingInfo& operator= (const VertexRoutingInfo& other) = default;
        VertexRoutingInfo& operator= (VertexRoutingInfo&& other) = default;
        ~VertexRoutingInfo() = default;
    };

	struct PriorityQueueMember {
		double cost;
		unsigned_id_type vertex_id;
		size_t hop_count;

		PriorityQueueMember(double c, unsigned_id_type v, size_t h) : cost(c), vertex_id(v), hop_count(h) {}

		PriorityQueueMember(const PriorityQueueMember& other) = default;
        PriorityQueueMember(PriorityQueueMember&& other) = default;
        PriorityQueueMember& operator= (const PriorityQueueMember& other) = default;
        PriorityQueueMember& operator= (PriorityQueueMember&& other) = default;
        ~PriorityQueueMember() = default;

		bool operator< (const PriorityQueueMember& other) {
            return cost < other.cost;
        }

        bool operator> (const PriorityQueueMember& other) {
            return cost > other.cost;
        }
	};

    struct PriorityQueueMemberMinComparator {
        bool operator() (const PriorityQueueMember& a , const PriorityQueueMember& b) {
            return a.cost > b.cost;
        }
    };


	bool IgnoreNeighbour(const Vertex& neighbour);

	void UpdateNeighbour(const PriorityQueueMember& min_member, const Edge& edge, PriorityQueue& q, const SearchRangeLimits& limits);

	// bool UpdateNeighbour(const VertexRoutingInfo& vertex_info, const Edge& edge, const SearchRangeLimits& limits);

};

template< typename G>
CHDijkstra<G>::CHDijkstra(G & g) : g_(g), touched_vertices_(), source_vertex_(0), contracted_vertex_(0) {}


template <typename G>
bool CHDijkstra<G>::Run(unsigned_id_type source_vertex, unsigned_id_type contracted_vertex, const SearchRangeLimits& limits) {
	assert(source_vertex != contracted_vertex);
	touched_vertices_.clear();
	source_vertex_ = source_vertex;
	contracted_vertex_ = contracted_vertex;
	touched_vertices_.insert_or_assign(source_vertex, VertexRoutingInfo{0, 0});
	PriorityQueue q{};
	q.emplace(0, source_vertex, 0);
	
	while(!q.empty()) {
		PriorityQueueMember min_member = q.top();
		q.pop();
		assert(touched_vertices_.contains(min_member.vertex_id));
		assert(min_member.hop_count < limits.max_hop_count);
		auto&& vertex = g_.GetVertex(min_member.vertex_id);
		auto&& vertex_info = touched_vertices_[min_member.vertex_id];
		bool queue_member_is_dead = vertex_info.cost < min_member.cost;
		if (queue_member_is_dead) {
			continue;
		}
		if (vertex_info.cost > limits.max_cost) {
			return true;
		}
		assert(vertex_info.cost == min_member.cost);
		
		for(auto&& edge : vertex.get_edges()) {
			UpdateNeighbour(min_member, edge, q, limits);
		}
	}
	return false;
}

template <typename G>
double CHDijkstra<G>::OneHopBackwardSearch(unsigned_id_type target_vertex_id) const {
	assert(GetPathLength(contracted_vertex_) == std::numeric_limits<double>::max());
	auto&& end_vertex = g_.GetVertex(target_vertex_id);

	double min_path_length = GetPathLength(target_vertex_id);
	for(auto&& redge : end_vertex.get_reverse_edges()) {
		double d = GetPathLength(redge.get_to());
		if (d == std::numeric_limits<double>::max()) {
			continue;
		}
		double path_length = d + redge.get_length(); 
		if (min_path_length > path_length) {
			min_path_length = path_length;
		}
	}
	return min_path_length;
}

template <typename G>
inline bool CHDijkstra<G>::IgnoreNeighbour(const Vertex& neighbour) {
	return neighbour.IsContracted() || neighbour.get_osm_id() == contracted_vertex_;
}

template <typename G>
void CHDijkstra<G>::UpdateNeighbour(const PriorityQueueMember& min_member, const Edge& edge, PriorityQueue& q, const SearchRangeLimits& limits) {
	unsigned_id_type neighbour_id = edge.get_to();
	auto&& neighbour_info = touched_vertices_[neighbour_id];
	double update_cost = min_member.cost + edge.get_length();
	if (update_cost < neighbour_info.cost && !IgnoreNeighbour(g_.GetVertex(neighbour_id)) ) {
		neighbour_info.cost = update_cost;
		neighbour_info.previous = min_member.vertex_id;
		size_t update_hop_count = min_member.hop_count + 1;
		if (update_hop_count < limits.max_hop_count) {
			q.emplace(neighbour_info.cost, neighbour_id, update_hop_count);
		}
	}
}

template <typename G>
double CHDijkstra<G>::GetPathLength(unsigned_id_type to) const {
	auto&& it = touched_vertices_.find(to);
	if (it != touched_vertices_.end()) {
		auto xx = it->second;
		return it->second.cost;
	}
	return std::numeric_limits<double>::max();
}

}
}
#endif //BACKEND_CH_DIJKSTRA_H