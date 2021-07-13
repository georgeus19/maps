#ifndef ROUTING_PREPROCESSING_INDEX_EXTENDER_H
#define ROUTING_PREPROCESSING_INDEX_EXTENDER_H

#include "routing/database/database_helper.h"

#include "routing/profile/preference_index.h"

#include <string>
#include <vector>
#include <stack>
#include <functional>
#include <fstream>

namespace routing{
namespace preprocessing{

template <typename Graph>
class IndexExtender{
    struct IndexValue;
public:
    IndexExtender(database::DatabaseHelper& d, Graph& graph) : d_(d), graph_(graph) {}

    void ExtendIndex(std::shared_ptr<profile::PreferenceIndex> data_index, const std::string& new_index_table);
private:
    std::reference_wrapper<database::DatabaseHelper> d_;
    std::reference_wrapper<Graph> graph_;

    struct IndexValue{
        unsigned_id_type uid;
        double value;

        IndexValue(unsigned_id_type u, double v) : uid(u), value(v) {}
    };

    double CalculateIndexValue(std::shared_ptr<profile::PreferenceIndex> data_index, typename Graph::Edge& input_edge);

    void SaveNewIndex(const std::string& table_name, const std::vector<IndexValue>& extended_index);
};

template <typename Graph>
void IndexExtender<Graph>::ExtendIndex(std::shared_ptr<profile::PreferenceIndex> data_index, const std::string& new_index_table) {
    std::vector<std::pair<unsigned_id_type, double>> extended_index{};
    graph_.get().ForEachEdge([&](typename Graph::Edge& edge){
        // Twoway is twice in the graph.
        bool twoway_condition = edge.IsTwoway() && edge.get_from() < edge.get_to();
        if (edge.IsForward() || twoway_condition) {
            if (edge.IsShortcut()) {
                extended_index.emplace_back(edge.get_uid(), CalculateIndexValue(data_index, edge));
            } else {
                extended_index.emplace_back(edge.get_uid(), data_index->GetOriginal(edge.get_uid()));
            }
        }
    });
    data_index->Create(d_.get(), extended_index, new_index_table);
}

template <typename Graph>
double IndexExtender<Graph>::CalculateIndexValue(std::shared_ptr<profile::PreferenceIndex> data_index, typename Graph::Edge& input_edge) {
    double index_value = 0;
    std::stack<typename Graph::Edge> shortcut_stack{};
    typename Graph::Edge edge = input_edge;
    while(!shortcut_stack.empty() || edge.IsShortcut()) {
        if (edge.IsShortcut()) {
            shortcut_stack.push(edge);
            edge = graph_.get().GetVertex(edge.get_from()).FindEdge([&](const typename Graph::Edge& e){
                return e.get_to() == edge.get_contracted_vertex(); 
            });
        } else {
            index_value += data_index->GetOriginal(edge.get_uid());
            edge = shortcut_stack.top();
            shortcut_stack.pop();
            // Do nothing with the shortcut itself - just get right child edge.

            edge = graph_.get().GetVertex(edge.get_contracted_vertex()).FindEdge([&](const typename Graph::Edge& e){
                return e.get_to() == edge.get_to(); 
            });
        }
    }
    // Last non-shortcut edge was not added due to empty stack and not being a shortcut.
    index_value += data_index->GetOriginal(edge.get_uid());
    return index_value;
}



}
}
#endif // ROUTING_PREPROCESSING_INDEX_EXTENDER_H