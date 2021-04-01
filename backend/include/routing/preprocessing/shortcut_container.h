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

template <typename Edge>
struct ShortcutContainer {
    std::vector<Edge> new_edges;
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