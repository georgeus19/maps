#include "routing/graph.h"
#include "routing/edge.h"
#include "routing/algorithm.h"
#include "routing/dijkstra.h"
#include "database_helper.h"
#include <string>

using namespace std;
using namespace routing;

const string kDbName = "gis";
const string kUser = "postgres";
const string kPassword = "wtz2trln";
const string kHostAddress = "127.0.0.1";
const string kPort = "5432";
void Test();
int main() {
Test();
    Dijkstra::G g{};
    DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    d.LoadGraph<Dijkstra::G>(13.3915000, 49.7262000, 2000, "cz_edges", g);
//49.7282850N, 13.3868150E   to 49.7230553N, 13.3998825E
    auto && r = d.FindClosestEdge(13.3998825, 49.7230553, "cz_edges");
    BasicEdge start{r};
    r = d.FindClosestEdge(13.3868150, 49.7282850, "cz_edges");
    BasicEdge end{r};

    Algorithm<Dijkstra> alg{g};
    auto && res = alg.Run(start.get_from(), end.get_to());
    for(auto&& e : res) {
        std::cout << e.from_ << "->" << e.to_ << std::endl;
    }
}

void Test() {
    Dijkstra::G g{};
    g.AddEdge(std::move(BasicEdge{0, 1, 2, 2}));
    g.AddEdge(std::move(BasicEdge{1, 1, 3, 2}));
    g.AddEdge(std::move(BasicEdge{2, 2, 6, 10}));
    g.AddEdge(std::move(BasicEdge{3, 3, 4, 3}));
    g.AddEdge(std::move(BasicEdge{4, 4, 3, 2}));
    g.AddEdge(std::move(BasicEdge{5, 4, 5, 2}));
    g.AddEdge(std::move(BasicEdge{6, 4, 6, 6}));
    g.AddEdge(std::move(BasicEdge{7, 5, 6, 2}));
    g.AddEdge(std::move(BasicEdge{8, 5, 3, 6}));

    Algorithm<Dijkstra> alg{g};
    auto && res = alg.Run(1, 6);
    for(auto&& e : res) {
        std::cout << e.from_ << "->" << e.to_ << std::endl;
    }
}