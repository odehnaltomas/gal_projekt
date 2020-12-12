#pragma once
#include<stack>
#include<list>
#include<map>
#include<ogdf/basic/Graph.h>
#include<ogdf/basic/GraphAttributes.h>

// Debug Macros
//#define DEBUG_ENABLED 1

#ifdef DEBUG_ENABLED
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_EXPR(x) do { std::cerr << #x << ": " << x << std::endl; } while (0)
#else 
#define DEBUG(...) do {} while (0)
#define DEBUG_EXPR(x) do {} while (0)
#endif
 
using namespace ogdf;

enum EdgeType {UNPROCESSED, TREE, BACK, FORWARD, CROSS};

namespace DFS
{
    typedef std::map<int, int> NodeOrderMapping;
    typedef std::map<int, EdgeType> EdgeTypeMapping;

    // DFS specific node colouring
    enum NodeColour {WHITE, GRAY, BLACK};

    class DFS
    {
    public:
        std::vector<node> nodes_pre;
        std::vector<node> nodes_post;

        std::map<int, int> branches;
        std::map<int, std::vector<node>> neighbors;
        std::map<int, std::vector<node>> reachable;

        std::vector<std::vector<node>> biconnected;

        std::map<int, std::list<int>> tree;

        DFS(const GraphAttributes& ga);
        
        node parent(node v);
        int pre(node v);
        int post(node v);
        int low1(node v);
        int low2(node v);

        EdgeType edgeType(edge e);
        EdgeType edgeType(adjEntry adj);
        void edgeType(adjEntry adj, EdgeType type);
    
    protected:
        const Graph& G;
        const GraphAttributes& GA;

        std::stack<node> nodes_depth;

        std::map<int, node> parents;

        NodeOrderMapping pre_order;
        NodeOrderMapping post_order;
        NodeOrderMapping lowpt1;
        NodeOrderMapping lowpt2;

        std::map<int, NodeColour> colours;
        EdgeTypeMapping edge_types;

        int timestamp;

        void StartNode(node v, node parent = nullptr);
        void FinishNode(node v, node parent = nullptr);
        
        void BuildReachable(node v);
        void BuildReachable(node v, node w, std::map<int, bool> &visited);
        
        void ExtractBiconnectedComponent(node v);

        void Visit(node v, node parent = nullptr);
    };
}

namespace GraphUtils
{
    std::vector<adjEntry> getBackEdges(node v, DFS::EdgeTypeMapping &edge_types);

    node getTargetNodeFor(node source, adjEntry adj);

    node LowPoint(node v, DFS::DFS &dfs, unsigned index = 0);
}

namespace Generic
{
    template<class T>
    void merge(std::vector<T> &vec1, std::vector<T> vec2);

    template<class T>
    void makeUnique(std::vector<T> &vec1);
}
