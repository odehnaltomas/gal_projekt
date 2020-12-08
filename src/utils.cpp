#include<limits>
#include<algorithm>
#include "utils.h"


namespace DFS
{
    DFS::DFS(const GraphAttributes& ga)
        : nodes_pre(), nodes_post(), neighbors(), reachable(), pre_order(), post_order(),
          tree(), G(ga.constGraph()), GA(ga), colours(), edge_types(), timestamp(0)
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

        for (auto v : this->nodes_pre) {
            BuildReachable(v);
        }
    }
        
    EdgeType DFS::edgeType(adjEntry adj)
    {
        return edge_types[adj->theEdge()->index()];
    }
        
    void DFS::edgeType(adjEntry adj, EdgeType type)
    {
        edge_types[adj->theEdge()->index()] = type;
    }
    
    void DFS::BuildReachable(node v)
    {
        DEBUG("calculating reachable nodes from %2d\n", v->index());

        auto visited = std::map<int, bool>();
        BuildReachable(v, v, visited);
        
        Generic::makeUnique(reachable[v->index()]);
    }
    
    void DFS::BuildReachable(node v, node w, std::map<int, bool> &visited)
    {
        if (visited[w->index()])
            return;

        visited[w->index()] = true;

        DEBUG("adding reachable node %2d\n", w->index());
        reachable[v->index()].push_back(w);

        DEBUG("processing neighbors of node %2d\n", w->index());
        DEBUG("{\n");
        for (auto w : neighbors[w->index()])
            BuildReachable(v, w, visited);
        DEBUG("}\n");
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
            //if (!adj->isSource())
            //    continue;

            if (edgeType(adj) != UNPROCESSED)
                continue;

            // get target node
            auto adjNode = GraphUtils::getTargetNodeFor(v, adj);
            // get target node's current color
            auto adjNodeColor = colours.at(adjNode->index());
            
            DEBUG("processing edge %2d (adj %2d)  |  %2d -> %2d\n",
                adj->theEdge()->index(), adj->index(), v->index(), adjNode->index());

            // save adjacent node as neighbor
            neighbors[v->index()].push_back(adjNode);
            
            // check
            if (adjNodeColor == WHITE) {
                // target node is white - it has just been discovered now
                // TREE EDGE
                edgeType(adj, TREE);
                DEBUG("  marking as TREE\n");

            } else if (adjNodeColor == GRAY) {
                // target node is gray - it is unfinished predecessor of this node
                // BACK EDGE - this closes cycle to its predecessor
                edgeType(adj, BACK);
                DEBUG("  marking as BACK\n");

            } else if (adjNodeColor == BLACK) {
                // target node is black - it has already been finished
                if (pre_order[adjNode->index()] > pre_order[v->index()]
                    && post_order[adjNode->index()] < post_order[v->index()]) {
                    // FORWARD EDGE iff target.DFI_in > my.DFI_in
                    //   && target.DFI_out < my.DFI_out
                    edgeType(adj, FORWARD);
                    DEBUG("  marking as FORWARD\n");

                } else {
                    // CORSS EDGE else
                    edgeType(adj, CROSS);
                    DEBUG("  marking as CROSS\n");

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

namespace GraphUtils
{
    std::vector<adjEntry> getBackEdges(node v, DFS::DFS &dfs) {
        auto result = std::vector<adjEntry>();

        adjEntry adj;
        // walk through edges connected to this node
        for(adj = v->firstAdj(); adj; adj = adj->succ()) {
            // only work with source edges
            //if (!adj->isSource())
            //    continue;
            
            if (dfs.edgeType(adj) == EdgeType::BACK)
                result.push_back(adj);
        }

        return result;
    }

    node getTargetNodeFor(node source, adjEntry adj) {
        if (adj->theNode()->index() == source->index()) {
            return adj->twinNode();
        }

        return adj->theNode();
    }

    node L(node v, DFS::DFS &dfs) {
        // L(v) = {min({v} join { u | EXISTS w such that there is a path from v to w
        //  and w has a backedge to u})}
        auto reachable = dfs.reachable[v->index()];
        auto result = std::vector<std::pair<node, int>>{
            {v, dfs.pre_order[v->index()]}
        };

        for (auto w : reachable) {
            DEBUG("processing node %d (reachable from %d)\n", w->index(), v->index());
            auto edges = getBackEdges(w, dfs);
            for (auto e : edges) {
                DEBUG("  found back edge %d\n", e->theEdge()->index());
                auto targetNode = e->twinNode();
                result.push_back({
                    targetNode,
                    dfs.pre_order[targetNode->index()]
                });
            }
        }

        if (result.size() == 0)
            return nullptr;

        else if (result.size() == 1)
            return (*result.begin()).first;

        struct {
            bool operator()(std::pair<node, int> a, std::pair<node, int> b) const {   
                return a.second < b.second;
            }   
        } customLess;
        std::sort(result.begin(), result.end(), customLess);

        return (*result.begin()).first;
    }
}

namespace Generic
{
    template<class T>
    void merge(std::vector<T> &vec1, std::vector<T> vec2) {
        vec1.insert(
            vec1.end(),
            vec2.begin(),
            vec2.end()
        );
    }
    
    template<class T>
    void makeUnique(std::vector<T> &vec1) {
        std::sort(vec1.begin(), vec1.end());
        std::unique(vec1.begin(), vec1.end());
    }
}
