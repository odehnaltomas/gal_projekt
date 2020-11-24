#pragma once
#include<list>
#include<ogdf/basic/Graph.h>
 
using namespace ogdf;

namespace DFS
{
    struct Entry {
        const int DFI_in;
        const int DFI_out;
        const node vertex;
    };
    typedef struct Entry Entry;

    enum Colour {WHITE, GRAY, BLACK};

    class DFS
    {
    public:
        std::list<Entry> result;

        DFS(const Graph& g);
    
    protected:
	    const Graph& g;
        std::list<Entry> entries;
        std::map<int, Colour> colours;
        int timestamp;

        int StartNode(node v);
        void FinishNode(node v, int timestamp_started);

        void Visit(node v);
    };
}
