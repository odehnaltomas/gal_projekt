#include<map>

#include "utils.h"


namespace DFS
{
    DFS::DFS(const Graph& g) : result(), g(g), entries(), colours(), timestamp(0)
    {
        // initialize node colours
        auto node = g.firstNode();
        do {
            colours[node->index()] = WHITE;
        }
        while ((node = node->succ()));
        
        // start creation of DFS trees
        node = g.firstNode();
        do {
            // only visit unprocessed nodes
            if (colours[node->index()] != WHITE)
                continue;
            
            // mark node as started
            const int DFI_in = StartNode(node);
            // visit node and recursively its adjacent nodes
            Visit(node);
            // mark node as finished
            FinishNode(node, DFI_in);
        }
        while ((node = node->succ()));
    }

    int DFS::StartNode(node v)
    {
        colours[v->index()] = GRAY;
        return timestamp++;
    }

    void DFS::FinishNode(node v, int timestamp_started)
    {
        colours[v->index()] = BLACK;
        entries.push_front({timestamp_started, timestamp++, v});
    }

    void DFS::Visit(node v)
    {
        adjEntry adj;
        // visit nodes directly adjacent to this node
        for(adj = v->firstAdj(); adj; adj = adj->succ()) {
            auto adjNode = adj->twinNode();
            // only visit unprocessed nodes
            if (colours.at(adjNode->index()) != WHITE)
                continue;
            
            // mark node as started
            const int DFI_in = StartNode(adjNode);
            // visit node and recursively its adjacent nodes
            Visit(adjNode);
            // mark node as finished
            FinishNode(adjNode, DFI_in);
        }
    }
}
