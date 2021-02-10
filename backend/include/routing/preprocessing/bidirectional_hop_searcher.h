#ifndef BACKEND_BIDIRECTIONAL_HOP_SEARCH_H
#define BACKEND_BIDIRECTIONAL_HOP_SEARCH_H

#include <vector>

namespace routing {
namespace preprocessing {

    template <typename Graph>
    class BidirectionalHopSearcher {
        Graph & g_;
    public:

        BidirectionalHopSearcher(Graph & g);

        void Search(size_t hop_limit);

    };

    template <typename Graph>
    BidirectionalHopSearcher<Graph>::BidirectionalHopSearcher(Graph & g)
        : g_(g) {}

    template <typename Graph>
    void BidirectionalHopSearcher<Graph>::Search(size_t hop_limit) {

    }


    
}
}

#endif //BACKEND_BIDIRECTIONAL_HOP_SEARCH_H
