#pragma once
#include<list>
#include<map>
#include<ogdf/basic/Graph.h>
#include<ogdf/basic/GraphAttributes.h>

// Debug Macros
#define DEBUG_ENABLED 1

#ifdef DEBUG_ENABLED
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_EXPR(x) do { std::cerr << #x << ": " << x << std::endl; } while (0)
#else 
#define DEBUG(x) do {} while (0)
#define DEBUG_EXPR(x) do {} while (0)
#endif
 
using namespace ogdf;

namespace DFS
{
    struct Entry {
        const int DFI_in;
        const int DFI_out;
        const node v;
    };
    typedef struct Entry Entry;
    
    typedef std::pair<node, node> NodePair;

    enum NodeColour {WHITE, GRAY, BLACK};

    enum EdgeType {TREE, BACK, FORWARD, CROSS};

    class DFS
    {
    public:
        std::vector<node> nodes_pre;
        std::vector<node> nodes_post;

        std::map<int, int> pre_order;
        std::map<int, int> post_order;

        std::map<int, EdgeType> edge_types;
        std::map<int, std::list<int>> tree;

        DFS(const GraphAttributes& ga);
    
    protected:
        const Graph& G;
        const GraphAttributes& GA;

        std::map<int, NodeColour> colours;
        int timestamp;

        void StartNode(node v);
        void FinishNode(node v, node parent = nullptr);

        void Visit(node v);
    };
}
