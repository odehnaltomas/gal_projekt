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
		if (this->dfs.edgeType(adj) == EdgeType::BACK && is_old(adj) == false) {
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
		if (this->dfs.edgeType(adj) == EdgeType::TREE && is_old(adj) == false) {
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
				auto Lw = GraphUtils::L(w, this->dfs);
				DEBUG("      }\n");
				DEBUG("      L(w) = node %d\n", Lw->index());
				
				DEBUG("      {\n");
				auto Lx = GraphUtils::L(x, this->dfs);
				DEBUG("      }\n");
				DEBUG("      L(x) = node %d\n", Lx->index());

				// find a new edge (w, x) with x = L(w) or L(x) = L(w)
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
		if (this->dfs.edgeType(adj) == EdgeType::BACK && is_old(adj) == false) {
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
				auto Lw = GraphUtils::L(w, this->dfs);
				DEBUG("      }\n");
				DEBUG("      L(w) = node %d\n", Lw->index());
				
				DEBUG("      {\n");
				auto Lx = GraphUtils::L(x, this->dfs);
				DEBUG("      }\n");
				DEBUG("      L(x) = node %d\n", Lx->index());

				// find a new edge (w, x) with x = L(w) or L(x) = L(w)
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
	auto t = this->G.firstNode();
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

bool AlgorithmBoyerMyrvold::isPlanar() {
	DEBUG_EXPR(this->GA.directed());
	auto dfsForest = dfs.nodes_pre;
	
	printf("DF-Tree:\n");
	for (auto entry : dfsForest) {
		auto pre = dfs.pre_order[entry->index()];
		auto post = dfs.post_order[entry->index()];
		auto low = dfs.lowpoint[entry->index()];
		printf("  %2s (%2d) = %3d, %3d, %3d\n", this->GA.label(entry).c_str(),
			entry->index(), pre, post, low);
	}
	
	printf("Bi-connected components:\n");
	for (auto component : dfs.biconnected) {
		printf("  Component contains:\n");
		for (auto entry : component) {
			auto pre = dfs.pre_order[entry->index()];
			auto post = dfs.post_order[entry->index()];
			auto low = dfs.lowpoint[entry->index()];
			printf("    %2s (%2d) = %3d, %3d, %3d\n", this->GA.label(entry).c_str(),
				entry->index(), pre, post, low);
		}
	}

	return false;
}
