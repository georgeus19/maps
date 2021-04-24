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
#include "robin_hood/robin_hood.h"
#include "tsl/robin_map.h"
#include "tsl/robin_set.h"

namespace routing {
namespace preprocessing {
/**
 * Implementation of dijkstra's algorithm modified for Contraction Hierarchies' local search.
 * There are lots of search limits or custom CH conditions. Especially function for one hop
 * backwards from target vertices of the search.
 */
template <typename G>
class CHDijkstra {
public:
    using Vertex = typename G::Vertex;
    using Edge = typename G::Edge;
    using Graph = G;
	using TargetVerticesMap = tsl::robin_map<unsigned_id_type, bool>;

    struct SearchRangeLimits {
        double max_cost;
        size_t max_hop_count;
		
		SearchRangeLimits(double c, size_t h) : max_cost(c), max_hop_count(h) {}

    };

    CHDijkstra(G & g);

	/**
	 * Main function of this class which initiates the dijkstra search.
	 * 
	 * @param source_vertex The vertex where the search begins.
	 * @param contracted_vertex The vertex that is about to be contracted - needs to be ingored.
	 * @param limits Limits for the local search - e.g. max cost
	 * @param target_vertices Vertices to which paths are found.
	 * @return True if local limits was triggered. False if no more vertices can be visited.
	 */
    bool Run(unsigned_id_type source_vertex, unsigned_id_type contracted_vertex, const SearchRangeLimits& limits, TargetVerticesMap& target_vertices);

	/**
	 * Get length of the shortest path to a vertex that was found before by running `Run` function.
	 */
    double GetPathLength(unsigned_id_type to) const;

	/**
	 * Perform one hop back search - scan all incoming edges of the target vertex
	 * and return the length of the path to the target vertex.
	 */
	double OneHopBackwardSearch(unsigned_id_type target_vertex_id) const;

	size_t GetSearchSpaceSize() const {
		return settled_vertices_;
	}
private:
    struct VertexRoutingProperties;
	struct PriorityQueueMember;
	struct PriorityQueueMemberMinComparator;
    using PriorityQueue = std::priority_queue<PriorityQueueMember, std::vector<PriorityQueueMember>, PriorityQueueMemberMinComparator>;
    G& g_;

	/**
	 * Stores all reached vertices from Run function. STL unordered map is not used,
	 * since slows the entire preprocessing wrt performance and memory usage.
	 */
	// using UnorderedMap = std::unordered_map<unsigned_id_type, VertexRoutingProperties>;
	// using UnorderedMap = robin_hood::unordered_map<unsigned_id_type, VertexRoutingProperties>;
	using UnorderedMap = tsl::robin_map<unsigned_id_type, VertexRoutingProperties>;
    UnorderedMap touched_vertices_;

    unsigned_id_type source_vertex_;
	unsigned_id_type contracted_vertex_;

	/**
	 * Number of visited vertices in Run function.
	 */
	size_t settled_vertices_;

	/**
	 * Stores information from the search. It is memory & performance inefficient to store it in vertices
	 * since a small portion of graph is searched.
	 */
    struct VertexRoutingProperties {
        double cost;
        unsigned_id_type previous;

		VertexRoutingProperties() : cost(std::numeric_limits<double>::max()), previous(0) {}

        VertexRoutingProperties(double c, unsigned_id_type p) : cost(c), previous(p) {}

        VertexRoutingProperties(const VertexRoutingProperties& other) = default;
        VertexRoutingProperties(VertexRoutingProperties&& other) = default;
        VertexRoutingProperties& operator= (const VertexRoutingProperties& other) = default;
        VertexRoutingProperties& operator= (VertexRoutingProperties&& other) = default;
        ~VertexRoutingProperties() = default;
    };

	/**
	 * Struct for priority queue used in Run function.
	 */
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

		bool operator< (const PriorityQueueMember& other) const {
            return cost < other.cost;
        }

        bool operator> (const PriorityQueueMember& other) const {
            return cost > other.cost;
        }
	};

    struct PriorityQueueMemberMinComparator {
        bool operator() (const PriorityQueueMember& a , const PriorityQueueMember& b) const {
            return a.cost > b.cost;
        }
    };

	/**
	 * Some vertices are always ingorned in the search - already contracted vertices, the to be contracted vertex.
	 */
	bool IgnoreNeighbour(const Vertex& neighbour);

	void UpdateNeighbour(const PriorityQueueMember& min_member, const Edge& edge, PriorityQueue& q, const SearchRangeLimits& limits);
};

template< typename G>
CHDijkstra<G>::CHDijkstra(G & g) : g_(g), touched_vertices_(1000), source_vertex_(0), contracted_vertex_(0), settled_vertices_(0) {}


template <typename G>
bool CHDijkstra<G>::Run(unsigned_id_type source_vertex, unsigned_id_type contracted_vertex, const SearchRangeLimits& limits, TargetVerticesMap& target_vertices) {
	assert(source_vertex != contracted_vertex);
	
	touched_vertices_.clear();
	source_vertex_ = source_vertex;
	settled_vertices_ = 0;
	contracted_vertex_ = contracted_vertex;
	touched_vertices_.insert_or_assign(source_vertex, VertexRoutingProperties{0, 0});
	PriorityQueue q{};
	q.emplace(0, source_vertex, 0);
	size_t dead_members = 0;
	size_t target_vertices_found = 0;
	while(!q.empty()) {
		++settled_vertices_;
		PriorityQueueMember min_member = q.top();
		q.pop();
		assert(touched_vertices_.contains(min_member.vertex_id));
		assert(min_member.hop_count < limits.max_hop_count);
		auto&& vertex = g_.GetVertex(min_member.vertex_id);
		VertexRoutingProperties vertex_routing_properties = touched_vertices_[min_member.vertex_id];
		
		// There might be more members in the `q` for one vertex - always use only the minimal one.
		bool queue_member_is_dead = vertex_routing_properties.cost < min_member.cost;
		if (queue_member_is_dead) {
			++dead_members;
			continue;
		}
		assert(vertex_routing_properties.cost == min_member.cost);
		auto&& it = target_vertices.find(min_member.vertex_id);
		if (it != target_vertices.end() && it->second == false) {
			target_vertices.insert_or_assign(min_member.vertex_id, true);
			++target_vertices_found; 
		}
		bool all_target_vertices_found = target_vertices_found == target_vertices.size();
		bool max_cost_surpassed = vertex_routing_properties.cost > limits.max_cost;
		if (all_target_vertices_found || max_cost_surpassed) {
			return true;
		}
		
		vertex.ForEachEdge([&](Edge& edge) {
			UpdateNeighbour(min_member, edge, q, limits);
		});
	}
	return false;
}

template <typename G>
double CHDijkstra<G>::OneHopBackwardSearch(unsigned_id_type target_vertex_id) const {
	assert(GetPathLength(contracted_vertex_) == std::numeric_limits<double>::max());
	auto&& end_vertex = g_.GetVertex(target_vertex_id);

	double min_path_length = GetPathLength(target_vertex_id);
	end_vertex.ForEachBackwardEdge([&](Edge& backward_edge) {
		double d = GetPathLength(backward_edge.get_backward_to());
		if (d == std::numeric_limits<double>::max()) {
			return;
		}
		double path_length = d + backward_edge.get_length(); 
		if (min_path_length > path_length) {
			min_path_length = path_length;
		}
	});
	return min_path_length;
}

template <typename G>
inline bool CHDijkstra<G>::IgnoreNeighbour(const Vertex& neighbour) {
	return neighbour.IsContracted() || neighbour.get_osm_id() == contracted_vertex_;
}

template <typename G>
void CHDijkstra<G>::UpdateNeighbour(const PriorityQueueMember& min_member, const Edge& edge, PriorityQueue& q, const SearchRangeLimits& limits) {
	unsigned_id_type neighbour_id = edge.get_to();
	auto&& neighbour_routing_properties = touched_vertices_[neighbour_id];
	double update_cost = min_member.cost + edge.get_length();
	if (update_cost < neighbour_routing_properties.cost && !IgnoreNeighbour(g_.GetVertex(neighbour_id)) ) {
		neighbour_routing_properties.cost = update_cost;
		neighbour_routing_properties.previous = min_member.vertex_id;
		size_t update_hop_count = min_member.hop_count + 1;
		if (update_hop_count < limits.max_hop_count) {
			q.emplace(neighbour_routing_properties.cost, neighbour_id, update_hop_count);
		}
	}
}

template <typename G>
double CHDijkstra<G>::GetPathLength(unsigned_id_type to) const {
	auto&& it = touched_vertices_.find(to);
	if (it != touched_vertices_.end()) {
		return it->second.cost;
	}
	return std::numeric_limits<double>::max();
}

}
}
#endif //BACKEND_CH_DIJKSTRA_H