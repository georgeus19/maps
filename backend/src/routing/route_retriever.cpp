#include "routing/route_retriever.h"
namespace routing {

BasicDirection::BasicDirection() : Direction() {}

BasicDirection::void SetCost(Vertex& vertex, double cost) override {
    vertex.set_cost(cost);
}

BasicDirection::double GetCost(Vertex& vertex) override {
    return vertex.get_cost();
}

BasicDirection::void SetPrevious(Vertex& vertex, unsigned_id_type previous) override {
    vertex.set_previous(previous);
}

BasicDirection::unsigned_id_type GetPrevious(Vertex& vertex) override {
    return vertex.get_previous();
}

BasicDirection::void ForEachEdge(Vertex& vertex, std::function<void(Edge&)> f) override {
    vertex.ForEachEdge(f);
}

BasicDirection::void Enqueue(double priority, unsigned_id_type vertex_id) override {
    throw NotImplementedException("Enqueue in BasicDirection is not implemented - it will never be!");
}











}