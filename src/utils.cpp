#include<limits>
#include<algorithm>
#include "utils.h"


namespace DFS
{
    DFS::DFS(const GraphAttributes& ga) : G(ga.constGraph()), GA(ga), timestamp(0)
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

            // make sure there are no component entries left
            ExtractBiconnectedComponent(n);
        }

        for (auto v : this->nodes_pre) {
            BuildReachable(v);
        }
    }
        
    int DFS::pre(node v) {
        return pre_order[v->index()];
    }

    int DFS::post(node v) {
        return post_order[v->index()];
    }
        
    int DFS::low1(node v) {
        return lowpt1[v->index()];
    }

    int DFS::low2(node v) {
        return lowpt2[v->index()];
    }
        
    EdgeType DFS::edgeType(edge e) {
        return edge_types[e->index()];
    }
        
    EdgeType DFS::edgeType(adjEntry adj) {
        return edgeType(adj->theEdge());
    }
        
    void DFS::edgeType(adjEntry adj, EdgeType type) {
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
        
    void DFS::ExtractBiconnectedComponent(node v)
    {
        DEBUG("%2s is AP, component contains:\n", GA.label(v).c_str());

        auto component = std::vector<node>();
        while(!nodes_depth.empty()) {
            auto top = nodes_depth.top();
            auto low = lowpt1[top->index()];
            auto pre = pre_order[v->index()];

            if (low >= pre) {
                // this node is part of the biconnected component
                DEBUG("  %2s (%2d >= %2d)\n", GA.label(top).c_str(), low, pre);
                component.push_back(top);
                nodes_depth.pop();

            } else {
                // this node is NOT part of the component anymore
                DEBUG("  break - %2s (%2d < %2d)\n", GA.label(top).c_str(), low, pre);
                break;

            }
        }

        if (component.size()) {
            //std::reverse(component.begin(), component.end());
            biconnected.push_back(component);
        }
    }

    void DFS::StartNode(node v, node parent)
    {
        auto v_id = v->index();
        auto parent_id = parent ? parent->index() : -1;

        // right before visit - set target node to gray as processing will start
        colours[v_id] = GRAY;
        // generate pre-order index
        pre_order[v_id] = timestamp++;

        // generate initial lowpoint values
        // statement A
        lowpt1[v_id] = lowpt2[v_id] = pre_order[v_id];

        // save this node as processed in pre-order
        nodes_pre.push_back(v);

        // append to parent's list of child nodes
        tree[parent_id].push_back(v_id);
        branches[parent_id]++;

        // create list of child nodes
        tree[v_id] = std::list<int>();
    }

    void DFS::FinishNode(node v, node parent)
    {
        auto v_id = v->index();
        auto parent_id = parent ? parent->index() : -1;

        // right after visit - set target node to black as processing has been finished
        colours[v_id] = BLACK;
        // generate post-order index
        post_order[v_id] = timestamp++;

        // save this node as processed in post-order
        nodes_post.push_back(v);

        // statement B
        if (lowpt1[v_id] < lowpt1[parent_id]) {
            // child's lowest point is lower than parent's
            // since the lowest point will be inherited from child, select
            //   new second lowest point as the lowest from parent's previous
            //   lowest point and child's second lowest point
            lowpt2[parent_id] = std::min(lowpt1[parent_id], lowpt2[v_id]);
            lowpt1[parent_id] = lowpt1[v_id];

        } else if (lowpt1[v_id] == lowpt1[parent_id]) {
            // child's lowest point is the same as parent's
            // if child's second lowest point is lower than parent's current
            //   second lowest, update it
            lowpt2[parent_id] = std::min(lowpt2[parent_id], lowpt2[v_id]);

        } else {
            // parent's lowest point is lower than child's
            // if child's lowest point is lower than parent's current
            //   second lowest, update it
            lowpt2[parent_id] = std::min(lowpt2[parent_id], lowpt1[v_id]);

        }
    }

    void DFS::Visit(node v, node parent)
    {
        nodes_depth.push(v);
        auto v_id = v->index();

        adjEntry w_adj;
        bool articulationPoint = false;

        // walk through edges connected to this node
        for(w_adj = v->firstAdj(); w_adj; w_adj = w_adj->succ()) {
            // only work with source edges
            //if (!adj->isSource())
            //    continue;

            if (edgeType(w_adj) != UNPROCESSED)
                continue;

            // get target node
            auto w = GraphUtils::getTargetNodeFor(v, w_adj);
            auto w_id = w->index();
            // get target node's current color
            auto w_colour = colours.at(w_id);
            
            DEBUG("processing edge %2d (adj %2d)  |  %2d -> %2d\n",
                w_adj->theEdge()->index(), w_adj->index(), v_id, w_id);

            // save adjacent node as neighbor
            neighbors[v_id].push_back(w);
            
            // check
            if (w_colour == WHITE) {
                // target node is white - it has just been discovered now
                // TREE EDGE
                edgeType(w_adj, TREE);
                DEBUG("  marking as TREE\n");

            } else if (w_colour == GRAY) {
                // target node is gray - it is unfinished predecessor of this node
                // BACK EDGE - this closes cycle to its predecessor
                edgeType(w_adj, BACK);
                DEBUG("  marking as BACK\n");

            } else if (w_colour == BLACK) {
                // target node is black - it has already been finished
                if (pre_order[w_id] > pre_order[v_id]
                    && post_order[w_id] < post_order[v_id]) {
                    // FORWARD EDGE iff target.DFI_in > my.DFI_in
                    //   && target.DFI_out < my.DFI_out
                    edgeType(w_adj, FORWARD);
                    DEBUG("  marking as FORWARD\n");

                } else {
                    // CORSS EDGE else
                    edgeType(w_adj, CROSS);
                    DEBUG("  marking as CROSS\n");

                }
            }

            // only visit unprocessed nodes
            if (w_colour == WHITE) {
                // mark node as started
                StartNode(w, v);
                // visit node and recursively its adjacent nodes
                Visit(w, v);
                // mark node as finished
                FinishNode(w, v);

                if (lowpt1[w_id] >= pre_order[v_id])
                    // parent might be articulation point
                    articulationPoint = true;

            } else {
                // adjacency to already processed node

                // statement C
                if (pre_order[w_id] < lowpt1[v_id]) {
                    lowpt2[v_id] = lowpt1[v_id];
                    lowpt1[v_id] = pre_order[w_id];

                } else if (pre_order[w_id] > lowpt1[v_id]) {
                    lowpt2[v_id] = std::min(lowpt2[v_id], pre_order[w_id]);

                }
            }
        }

        if ((parent != nullptr && articulationPoint)
                || (parent == nullptr && branches[v_id] > 1)) {
            ExtractBiconnectedComponent(v);
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

    node LowPoint(node v, DFS::DFS &dfs, unsigned index) {
        // LowPoint(v) = {min({v} join { u | EXISTS w such that there is a path from v to w
        //  and w has a backedge to u})}
        auto reachable = dfs.reachable[v->index()];
        auto result = std::vector<std::pair<node, int>>{
            {v, dfs.pre(v)}
        };

        for (auto w : reachable) {
            DEBUG("processing node %d (reachable from %d)\n", w->index(), v->index());
            auto edges = getBackEdges(w, dfs);
            for (auto e : edges) {
                DEBUG("  found back edge %d\n", e->theEdge()->index());
                auto targetNode = e->twinNode();
                result.push_back({
                    targetNode,
                    dfs.pre(targetNode)
                });
            }
        }

        struct {
            bool operator()(std::pair<node, int> a, std::pair<node, int> b) const {   
                return a.second < b.second;
            }   
        } customLess;
        std::sort(result.begin(), result.end(), customLess);
        std::unique(result.begin(), result.end());

        if (result.size() < index + 1)
            return v;

        return result[index].first;
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
