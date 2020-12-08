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

enum EdgeType {UNPROCESSED, TREE, BACK, FORWARD, CROSS};

namespace DFS
{
    struct Entry {
        const int DFI_in;
        const int DFI_out;
        const node v;
    };
    typedef struct Entry Entry;
    
    typedef std::map<int, int> NodeOrderMapping;
    typedef std::map<int, EdgeType> EdgeTypeMapping;

    // DFS specific node colouring
    enum NodeColour {WHITE, GRAY, BLACK};

    class DFS
    {
    public:
        std::vector<node> nodes_pre;
        std::vector<node> nodes_post;
        std::map<int, std::vector<node>> neighbors;
        std::map<int, std::vector<node>> reachable;

        NodeOrderMapping pre_order;
        NodeOrderMapping post_order;

        std::map<int, std::list<int>> tree;

        DFS(const GraphAttributes& ga);
        
        EdgeType edgeType(adjEntry adj);
        void edgeType(adjEntry adj, EdgeType type);
    
    protected:
        const Graph& G;
        const GraphAttributes& GA;

        std::map<int, NodeColour> colours;
        EdgeTypeMapping edge_types;

        int timestamp;

        void StartNode(node v);
        void FinishNode(node v, node parent = nullptr);
        
        void BuildReachable(node v);
        void BuildReachable(node v, node w, std::map<int, bool> &visited);

        void Visit(node v);
    };
}

namespace GraphUtils
{
    std::vector<adjEntry> getBackEdges(node v, DFS::EdgeTypeMapping &edge_types);

    node getTargetNodeFor(node source, adjEntry adj);

    node L(node v, DFS::DFS &dfs);
}

namespace Generic
{
    template<class T>
    void merge(std::vector<T> &vec1, std::vector<T> vec2);

    template<class T>
    void makeUnique(std::vector<T> &vec1);
}
