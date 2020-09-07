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

int main() {

    Dijkstra::G g{};
    DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    d.LoadGraph<Dijkstra::G>(13.3915000, 49.7262000, 2000, "cz_edges", g);
//49.7282850N, 13.3868150E   to 49.7230553N, 13.3998825E
    auto && r = d.FindClosestEdge(13.3868150, 49.7282850, "cz_edges");
    BasicEdge start{
            r.get<unsigned_id_type>(0),
            r.get<std::string>(1),
            r.get<unsigned_id_type>(2),
            r.get<unsigned_id_type>(3),
            r.get<double>(4)
    };
    r = d.FindClosestEdge(13.3998825, 49.7230553, "cz_edges");
    BasicEdge end{
            r.get<unsigned_id_type>(0),
            r.get<std::string>(1),
            r.get<unsigned_id_type>(2),
            r.get<unsigned_id_type>(3),
            r.get<double>(4)
    };
    Algorithm<Dijkstra> alg{g};
    auto && res = alg.Run(start.get_from(), end.get_from());
    for(auto&& e : res) {
        std::cout << e.geography_ << std::endl;
    }


}