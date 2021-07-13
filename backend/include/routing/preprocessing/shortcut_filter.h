#ifndef ROUTING_PREPROCESSING_SHORTCUT_FILTER_H
#define ROUTING_PREPROCESSING_SHORTCUT_FILTER_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/utility/comparison.h"
#include "routing/types.h"

#include <vector>
#include <iterator>
#include <set>
#include <queue>
#include <cassert>

namespace routing {
namespace preprocessing {

/**
 * Class with the functionality for filtering useless new shortcuts and classifying the new shortcuts
 * to `ShortcutContainer`.
 * @see ShortcutContainer
 */
template <typename Graph>
class ShortcutFilter {
    using Vertex = typename Graph::Vertex;
    using Edge = typename Graph::Edge;
public:
    ShortcutFilter(Graph& g);

    /**
     * Filter out shortcuts from `shortcuts` which are in the vector multiple times - only 
     * the edge with the smallest length is preserved.
     */
    std::vector<Edge> FilterDuplicateShortcuts(const std::vector<Edge>& shortcuts);

    std::vector<Edge> MergeUnorderedShortcuts(const std::vector<Edge>& shortcuts);
    
private:
    Graph& g_;

    void IsTwoway(Edge& shortcut, const Edge& other_shortcut, bool& filter);

    bool IsDuplicate(Edge& shortcut, const Edge& other_shortcut, bool& filter);

};


template <typename Graph>
ShortcutFilter<Graph>::ShortcutFilter(Graph& g) : g_(g) {}

template <typename Graph>
std::vector<typename ShortcutFilter<Graph>::Edge> ShortcutFilter<Graph>::FilterDuplicateShortcuts(const std::vector<Edge>& shortcuts) {
    std::vector<Edge> unique_shortcuts;
    unique_shortcuts.reserve(shortcuts.size());
    for(auto&& s : shortcuts) {
        Edge shortcut{s};
        bool filter = false;
        for(auto&& other_shortcut : shortcuts) {
            IsDuplicate(shortcut, other_shortcut, filter);
        }
        if (!filter) {
            unique_shortcuts.push_back(std::move(shortcut));
        }
    }
    return unique_shortcuts;
}

template <typename Graph>
std::vector<typename ShortcutFilter<Graph>::Edge> ShortcutFilter<Graph>::MergeUnorderedShortcuts(const std::vector<Edge>& shortcuts) {
    std::vector<Edge> unique_shortcuts;
    unique_shortcuts.reserve(shortcuts.size());
    for(auto&& s : shortcuts) {
        Edge shortcut{s};
        bool filter = false;
        for(auto&& other_shortcut : shortcuts) {
            IsTwoway(shortcut, other_shortcut, filter);
        }
        if (!filter) {
            unique_shortcuts.push_back(std::move(shortcut));
        }
    }
    return unique_shortcuts;
}

template <typename Graph>
void ShortcutFilter<Graph>::IsTwoway(Edge& shortcut, const Edge& other_shortcut, bool& filter) {
    bool reverse_edge = shortcut.get_from() == other_shortcut.get_to() && shortcut.get_to() == other_shortcut.get_from();
    bool same_length = utility::AreEqual<float>(shortcut.get_length(), other_shortcut.get_length());
    if (reverse_edge && same_length) {
        if (shortcut.get_from() < shortcut.get_to()) {
            shortcut.SetTwoway();
        } else {
            filter = true;
        }
    }
}

template <typename Graph>
bool ShortcutFilter<Graph>::IsDuplicate(Edge& shortcut, const Edge& other_shortcut, bool& filter) {
    bool same_origin = shortcut.get_from() == other_shortcut.get_from();
    bool same_destination = shortcut.get_to() == other_shortcut.get_to();
    bool not_the_same_edge = shortcut.get_uid() != other_shortcut.get_uid(); 
    
    if (same_origin && same_destination && not_the_same_edge) {
        if (shortcut.get_length() > other_shortcut.get_length()) {
            filter = true;
        } else if (utility::AreEqual<float>(shortcut.get_length(), other_shortcut.get_length())) {
            if (shortcut.get_uid() > other_shortcut.get_uid()) {
                filter = true;
            }
        }
    }
    return filter;
}



}
}

#endif //ROUTING_PREPROCESSING_SHORTCUT_FILTER_H