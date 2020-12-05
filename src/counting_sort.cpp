#include "counting_sort.h"

using namespace ogdf;

namespace CntSort
{
    void CountingSort::sort(GraphAttributes& GA, Graph& G, NodeArray<std::vector<edge>>& nodeOutEdges, EdgeArray<int>& nestingDepth) {
        (void)GA;
//        for (node n : G.nodes) {
//            std::cout << std::endl << std::endl << "NODE: " << GA.label(n);
//            for (edge e : nodeOutEdges[n]) {
//                std::cout << "\tedge id: " << GA.label(e) << ", nestingDepth: " << nestingDepth[e] << std::endl;
//                std::cout << "\t\tsource: " << GA.label(e->source()) << ", target: " << GA.label(e->target()) << std::endl;
//            }
//        }

        for (node n : G.nodes) {
            CountingSort::sortNode(nodeOutEdges[n], nestingDepth);
        }
        std::cout << "--------------COUNTING SORT DONE--------------" << std::endl;

//        for (node n : G.nodes) {
//            std::cout << std::endl << std::endl << "NODE: " << GA.label(n);
//            for (edge e : nodeOutEdges[n]) {
//                std::cout << "\tedge id: " << GA.label(e) << ", nestingDepth: " << nestingDepth[e] << std::endl;
//            }
//        }
    }

    void CountingSort::sortNode(std::vector<edge>& edges, EdgeArray<int>& nestingDepth) {
        int edgesSize = edges.size();
        int max = 0;
        for (edge e : edges) {
            if (max < nestingDepth[e]) {
                max = nestingDepth[e];
            }
        }
        int range = max + 1;

        std::vector<int> count(range);
        std::vector<edge> output(edgesSize);
        for (int i = 0; i < edgesSize; i++) {
            count[nestingDepth[edges[i]]]++;
        }

        for (unsigned int i = 1; i < count.size(); i++) {
            count[i] += count[i - 1];
        }

        for (int i = edgesSize - 1; i >= 0; i--) {
            output[count[nestingDepth[edges[i]]] - 1] = edges[i];
            count[nestingDepth[edges[i]]]--;
        }

        for (int i = 0; i < edgesSize; i++) {
            edges[i] = output[i];
        }
    }
}
