#pragma once
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/EdgeArray.h>

using namespace ogdf;

namespace CntSort
{
    class CountingSort
    {
    public:
        static void sort(GraphAttributes& GA, Graph& G, NodeArray<std::vector<edge>>& nodeOutEdges, EdgeArray<int>& nestingDepth);

    private:
        static void sortNode(std::vector<edge>& edges, EdgeArray<int>& nestingDepth);
    };
}
