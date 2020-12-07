#include<limits>
#include "utils.h"


namespace DFS
{
    DFS::DFS(const GraphAttributes& ga) : nodes_pre(), nodes_post(), pre_order(), post_order(), edge_types(), tree(), G(ga.constGraph()), GA(ga), colours(), timestamp(0)
    {
        // initialize node colours
        for (auto n : this->G.nodes) {
            colours[n->index()] = WHITE;
            pre_order[n->index()] = std::numeric_limits<int>::max();
            post_order[n->index()] = std::numeric_limits<int>::max();
        }
        
        // start creation of DFS trees
        for (auto n : this->G.nodes) {
            // only visit unprocessed nodes
            if (colours[n->index()] != WHITE)
                continue;
            
            // mark node as started
            StartNode(n);
            // visit node and recursively its adjacent nodes
            Visit(n);
            // mark node as finished
            FinishNode(n);
        }
    }

    void DFS::StartNode(node v)
    {
        // right before visit - set target node to gray as processing will start
        colours[v->index()] = GRAY;
        // generate pre-order index
        pre_order[v->index()] = timestamp++;
        // create list of child nodes
        tree[v->index()] = std::list<int>();
        // save this node as processed in pre-order
        nodes_pre.push_back(v);
    }

    void DFS::FinishNode(node v, node parent)
    {
        // right after visit - set target node to black as processing has been finished
        colours[v->index()] = BLACK;
        // generate post-order index
        post_order[v->index()] = timestamp++;
        // append to parent's list of child nodes
        tree[parent ? parent->index() : -1].push_back(v->index());
        // save this node as processed in post-order
        nodes_post.push_back(v);
    }

    void DFS::Visit(node v)
    {
        adjEntry adj;
        // walk through edges connected to this node
        for(adj = v->firstAdj(); adj; adj = adj->succ()) {
            // only work with source edges
            if (this->GA.directed() && !adj->isSource())
                continue;

            // get target node
            auto adjNode = adj->twinNode();
            // get target node's current color
            auto adjNodeColor = colours.at(adjNode->index());
            
            // check
            if (adjNodeColor == WHITE) {
                // target node is white - it has just been discovered now
                // TREE EDGE
                edge_types[adj->index()] = TREE;

            } else if (adjNodeColor == GRAY) {
                // target node is gray - it is unfinished predecessor of this node
                // BACK EDGE - this closes cycle to its predecessor
                edge_types[adj->index()] = BACK;

            } else if (adjNodeColor == BLACK) {
                // target node is black - it has already been finished
                if (pre_order[adjNode->index()] > pre_order[v->index()]
                    && post_order[adjNode->index()] < post_order[v->index()]) {
                    // FORWARD EDGE iff target.DFI_in > my.DFI_in
                    //   && target.DFI_out < my.DFI_out
                    edge_types[adj->index()] = FORWARD;

                } else {
                    // CORSS EDGE else
                    edge_types[adj->index()] = CROSS;

                }
            }

            // only visit unprocessed nodes
            if (adjNodeColor == WHITE) {
                // mark node as started
                StartNode(adjNode);
                // visit node and recursively its adjacent nodes
                Visit(adjNode);
                // mark node as finished
                FinishNode(adjNode, v);
            }
        }
    }
}
