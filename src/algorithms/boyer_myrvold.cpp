#include<stack>
#include "boyer_myrvold.h"


using namespace ogdf;

AlgorithmBoyerMyrvold::AlgorithmBoyerMyrvold(Graph& g, GraphAttributes& ga)
	: G(g), GA(ga), dfs(DFS::DFS(ga)), edge_old(), node_old() {}
	
void AlgorithmBoyerMyrvold::mark_old(node v) {
	node_old[v->index()] = true;
}

void AlgorithmBoyerMyrvold::mark_old(adjEntry adj) {
	edge_old[adj->theEdge()->index()] = true;
	// TODO: Both ways in undirected graph?
}
	
bool AlgorithmBoyerMyrvold::is_old(node v) {
	return node_old[v->index()];
}

bool AlgorithmBoyerMyrvold::is_old(adjEntry adj) {
	return edge_old[adj->theEdge()->index()];
}

std::vector<std::pair<node, adjEntry>> AlgorithmBoyerMyrvold::pathfinder(node v) {
	adjEntry adj;
	auto path = std::vector<std::pair<node, adjEntry>>();

	DEBUG("looking for BACK (cycle) edges from %d\n", v->index());
	// visit nodes directly adjacent to this node
	for(adj = v->firstAdj(); adj; adj = adj->succ()) {
		// only work with source edges
		//if (!adj->isSource())
		//	continue;

		// if there exists a cycle edge (v, w) with w*->v
		if (dfs.edgeType(adj) == EdgeType::BACK && is_old(adj) == false) {
			DEBUG("  processing BACK (cycle) edge %d\n", adj->theEdge()->index());

			// mark (v, w) old
			mark_old(adj);

			// let path be (v, w)
			path.push_back({v, adj});

			return path;
		}
	}

	DEBUG("looking for new TREE edges from %d\n", v->index());
	// visit nodes directly adjacent to this node
	for(adj = v->firstAdj(); adj; adj = adj->succ()) {
		// only work with source edges
		//if (!adj->isSource())
		//	continue;

		// get edge's target node
		auto w = GraphUtils::getTargetNodeFor(v, adj);

		// if there is a new tree edge v->w
		if (dfs.edgeType(adj) == EdgeType::TREE && is_old(adj) == false) {
			DEBUG("  processing new TREE edge %d (%d -> %d)\n",
				adj->theEdge()->index(), v->index(), w->index());

			// mark (v, w) old
			mark_old(adj);

			// initialize path to be (v, w)
			path.push_back({v, adj});
			
			auto subadj = w->firstAdj();
			while (is_old(w) == false && subadj) {
				// only work with source edges
				//if (!subadj->isSource()) {
				//	subadj = subadj->succ();
				//	continue;
				//}

				auto x = GraphUtils::getTargetNodeFor(w, subadj);

				if (is_old(subadj)) {
					DEBUG("    skipping old edge %d (%d -> %d)\n",
						subadj->theEdge()->index(), w->index(), x->index());

					subadj = subadj->succ();
					continue;
				}

				DEBUG("    processing new edge %d (%d -> %d)\n",
					subadj->theEdge()->index(), w->index(), x->index());

				DEBUG("      {\n");
				auto Lw = GraphUtils::LowPoint(w, dfs);
				DEBUG("      }\n");
				DEBUG("      LowPoint(w) = node %d\n", Lw->index());
				
				DEBUG("      {\n");
				auto Lx = GraphUtils::LowPoint(x, dfs);
				DEBUG("      }\n");
				DEBUG("      LowPoint(x) = node %d\n", Lx->index());

				// find a new edge (w, x) with x = LowPoint(w) or LowPoint(x) = LowPoint(w)
				if (x->index() != Lw->index() && Lx->index() != Lw->index()) {
					DEBUG("      skipping this edge - target node doesn't match\n");
					subadj = subadj->succ();
					continue;
				}

				// mark w old
				mark_old(w);
				// mark (w, x) old
				mark_old(subadj);

				// add (w, x) to path
				path.push_back({w, subadj});
				DEBUG("      adding this edge to the path\n");

				// w := x
				w = x;
				subadj = w->firstAdj();
			}
			
			return path;
		}
	}

	DEBUG("looking for new BACK (cycle) edges from %d\n", v->index());
	// visit nodes directly adjacent to this node
	for(adj = v->firstAdj(); adj; adj = adj->succ()) {
		// only work with NON-source edges!
		//if (adj->isSource())
		//	continue;

		// get edge's target node
		auto w = GraphUtils::getTargetNodeFor(v, adj);

		// if there is a new cycle edge v*->w
		if (dfs.edgeType(adj) == EdgeType::BACK && is_old(adj) == false) {
			DEBUG("  processing new BACK (cycle) edge %d (%d -> %d)\n",
				adj->theEdge()->index(), v->index(), w->index());

			// mark (v, w) old
			mark_old(adj);

			// initialize path to be (v, w)
			path.push_back({v, adj});

			auto subadj = w->firstAdj();
			while (is_old(w) == false && subadj) {
				// only work with NON-source edges!
				//if (subadj->isSource()) {
				//	subadj = subadj->succ();
				//	continue;
				//}

				// get edge's target node
				auto x = GraphUtils::getTargetNodeFor(w, subadj);
				
				if (is_old(subadj)) {
					DEBUG("    skipping old edge %d (%d -> %d)\n",
						subadj->theEdge()->index(), w->index(), x->index());

					subadj = subadj->succ();
					continue;
				}

				DEBUG("    processing new edge %d (%d -> %d)\n",
					subadj->theEdge()->index(), w->index(), x->index());

				DEBUG("      {\n");
				auto Lw = GraphUtils::LowPoint(w, dfs);
				DEBUG("      }\n");
				DEBUG("      LowPoint(w) = node %d\n", Lw->index());
				
				DEBUG("      {\n");
				auto Lx = GraphUtils::LowPoint(x, dfs);
				DEBUG("      }\n");
				DEBUG("      LowPoint(x) = node %d\n", Lx->index());

				// find a new edge (w, x) with x = LowPoint(w) or LowPoint(x) = LowPoint(w)
				if (x->index() != Lw->index() && Lx->index() != Lw->index()) {
					DEBUG("      skipping this edge - target node doesn't match\n");
					subadj = subadj->succ();
					continue;
				}

				// mark w and (w, x) old
				mark_old(w);
				mark_old(subadj);

				// add (w, x) to path
				path.push_back({w, subadj});

				// w := x
				w = x;
				subadj = w->firstAdj();
			}
			
			return path;
		}
	}

	return path;
}

std::map<int, int> AlgorithmBoyerMyrvold::stNumbering() {
	auto t = G.firstNode();
	auto ts_adj = t->firstAdj();
	auto s = GraphUtils::getTargetNodeFor(t, ts_adj);

	auto result = std::map<int, int>();
	auto vertices = std::stack<node>();

	// mark `t`, `s` and (t, s) old
	mark_old(t);
	mark_old(s);
	mark_old(ts_adj);

	// initialize stack to contain `s` on top of `t`
	vertices.push(t);
	vertices.push(s);

	int i = 0;
	// while stack is not empty
	while (vertices.empty() == false) {
		// let v be the top vertex of stack
		auto v = vertices.top();
		// delete v from stack
		vertices.pop();
		DEBUG("processing stack-top node %d\n", v->index());

		// let (v_1, v_2), ..., (v_k-1, v_k) be path found by pathfinder(v)
		DEBUG("[\n");
		auto path = pathfinder(v);
		DEBUG("]\n");

		DEBUG_EXPR(path.size());
		for (const auto entry : path) {
			auto v = entry.first;
			auto w = GraphUtils::getTargetNodeFor(v, entry.second);
			DEBUG("(%d -> %d) ", v->index(), w->index());
		}
		DEBUG("\n");

		// if path not "null"
		if (path.empty() == false) {
			// reverse path
			std::reverse(path.begin(), path.end());
			// add vertices to stack, from v_k-1 to v1 (v1 = v)
			for (auto entries : path) {
				vertices.push(entries.first);
			}

		} else {
			// map v to i := i + 1
			result[v->index()] = ++i;
		}
	}

	return result;
}

std::vector<std::pair<int, adjEntry>> AlgorithmBoyerMyrvold::orderedEdges() {
auto edges = std::vector<std::pair<int, adjEntry>>();
	for (auto e : G.edges) {
		DEBUG("calculating value of edge %2s\n", GA.label(e).c_str());
		auto adj = e->adjSource();
		auto v = adj->theNode();
		auto w = adj->twinNode();

		int val = -1;
		if (dfs.edgeType(adj) == EdgeType::BACK) {
			DEBUG("  this is BACK edge (%2s -> %2s)\n", GA.label(v).c_str(), GA.label(w).c_str());
			if (dfs.pre(v) < dfs.pre(w)) {
				// v has been discovered BEFORE w
				// since this is BACK edge - v surely is ancestor of w
				std::swap(v, w);
				DEBUG("    correction to (%2s -> %2s)\n", GA.label(v).c_str(), GA.label(w).c_str());
				adj = e->adjTarget();
			}
			val = 2 * dfs.pre(w);

		} else if (dfs.edgeType(adj) == EdgeType::TREE
				&& dfs.low2(w) >= dfs.pre(v)) {
			val = 2 * dfs.low1(w);

		} else if (dfs.edgeType(adj) == EdgeType::TREE
				&& dfs.low2(w) < dfs.pre(v)) {
			val = 2 * dfs.low1(w) + 1;

		}

		edges.push_back({val, adj});
	}
	
	struct {
		bool operator()(std::pair<int, adjEntry> a, std::pair<int, adjEntry> b) const {   
			return a.first < b.first;
		}   
	} customLess;
	std::sort(edges.begin(), edges.end(), customLess);

	return edges;
}

bool AlgorithmBoyerMyrvold::isPlanar() {
	DEBUG_EXPR(GA.directed());
	auto dfsForest = dfs.nodes_pre;
	
	DEBUG("DF-Tree:\n");
	for (auto v : dfsForest) {
		auto pre = dfs.pre(v);
		auto post = dfs.post(v);
		auto lowpt1 = dfs.low1(v);
		auto lowpt2 = dfs.low2(v);
		DEBUG("    %2s (%2d) = %3d, %3d, L1: %3d, L2: %3d\n",
			GA.label(v).c_str(), v->index(), pre, post, lowpt1, lowpt2);
	}
	
	DEBUG("Bi-connected components:\n");
	for (auto component : dfs.biconnected) {
		DEBUG("  Component contains:\n");
		for (auto v : component) {
			auto pre = dfs.pre(v);
			auto post = dfs.post(v);
			auto lowpt1 = dfs.low1(v);
			auto lowpt2 = dfs.low2(v);
			DEBUG("    %2s (%2d) = %3d, %3d, L1: %3d, L2: %3d\n",
				GA.label(v).c_str(), v->index(), pre, post, lowpt1, lowpt2);
		}
	}

	auto edges = orderedEdges();
	DEBUG("Ordered edges:\n");
	for (auto entry : edges) {
		auto v = entry.second->theNode();
		auto w = entry.second->twinNode();

		ordered_adjs[v->index()].push_back(entry.second);
		ordered_adjs[w->index()].push_back(entry.second->twin());
		
		DEBUG("  %2d: %2s (%2d) -> %2s (%2d)  %s\n", entry.first,
			GA.label(v).c_str(), v->index(),
			GA.label(w).c_str(), w->index(),
			dfs.edgeType(entry.second) == EdgeType::BACK ? "IS BACK" : "");
	}

	

	if (G.numberOfEdges() > 3 * G.numberOfNodes() - 3
			|| (G.numberOfNodes() >= 3 && G.numberOfEdges() > 3 * G.numberOfNodes() - 6)) {
		DEBUG("PLANARITY PRECONDITION FAILED!");
		return false;
	}

	return false;
}
