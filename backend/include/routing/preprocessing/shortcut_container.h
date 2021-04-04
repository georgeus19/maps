#ifndef BACKEND_SHORTCUT_CONTAINER_H
#define BACKEND_SHORTCUT_CONTAINER_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include <vector>
#include <iterator>
#include <set>
#include <queue>
#include <cassert>
#include "routing/preprocessing/contraction_parameters.h"

namespace routing {
namespace preprocessing {

/**
 * Container for new shortcut edges.
 */
template <typename Edge>
struct ShortcutContainer {
    /**
     * Edges that are not in the graph yet.
     */
    std::vector<Edge> new_edges;
    /**
     * Edges that are in the graph but these have lower lengths.
     */
    std::vector<Edge> improving_edges;
    ShortcutContainer() : new_edges(), improving_edges() {}
    ShortcutContainer(std::vector<Edge> && ne, std::vector<Edge>&& ie) : new_edges(std::move(ne)), improving_edges(std::move(ie)) {}
    ShortcutContainer(const ShortcutContainer& other) = default;
    ShortcutContainer(ShortcutContainer&& other) = default; 
    ShortcutContainer& operator=(const ShortcutContainer& other) = default;
    ShortcutContainer& operator=(ShortcutContainer&& other) = default;
    ~ShortcutContainer() = default;
};


}
}

#endif //BACKEND_SHORTCUT_CONTAINER_H