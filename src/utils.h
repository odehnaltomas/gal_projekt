#pragma once
#include<list>
#include<map>
#include<ogdf/basic/Graph.h>
 
using namespace ogdf;

namespace DFS
{
    struct Entry {
        const int DFI_in;
        const int DFI_out;
        const node v;
    };
    typedef struct Entry Entry;

    enum Colour {WHITE, GRAY, BLACK};

    class DFS
    {
    public:
        std::list<Entry> result;
        std::map<int, std::list<int>> tree;

        DFS(const Graph& g);

        void NodesPostOrder(std::list<int>& nodes);
    
    protected:
        const Graph& g;
        std::list<Entry> entries;
        std::map<int, Colour> colours;
        int timestamp;

        int StartNode(node v);
        void FinishNode(node v, int timestamp_started, node parent = nullptr);

        void Visit(node v);
    };
}
