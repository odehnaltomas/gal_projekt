#include<stack>
#include<ogdf/basic/extended_graph_alg.h>
#include "hopcroft_tarjan.h"


using namespace ogdf;

AlgorithmHopcroftTarjan::AlgorithmHopcroftTarjan(Graph& g, GraphAttributes& ga)
	: G(g), GA(ga), dfs(DFS::DFS(ga)) {}

std::list<adjEntry>* AlgorithmHopcroftTarjan::edgeOfCycle(adjEntry adj) {
	return edge_cycle[adj->theEdge()->index()];
}

void AlgorithmHopcroftTarjan::edgeOfCycle(adjEntry adj, std::list<adjEntry>* cycle) {
	edge_cycle[adj->theEdge()->index()] = cycle;
}

std::vector<std::pair<int, adjEntry>> AlgorithmHopcroftTarjan::orderedEdges() {
auto edges = std::vector<std::pair<int, adjEntry>>();
	for (auto e : G.edges) {
		DEBUG("calculating value of edge %2s\n", GA.label(e).c_str());
		auto adj = e->adjSource();
		auto v = adj->theNode();
		auto w = adj->twinNode();
		
		DEBUG("  processing edge (%2s -> %2s)\n", GA.label(v).c_str(), GA.label(w).c_str());
		if (dfs.edgeType(adj) == EdgeType::TREE && dfs.pre(v) > dfs.pre(w)) {
			// v has been discovered AFTER w, this DF-Tree edge must be reverted
			std::swap(v, w);
			DEBUG("    correcting to (%2s -> %2s)\n", GA.label(v).c_str(), GA.label(w).c_str());
			adj = e->adjTarget();
			
		} else if (dfs.edgeType(adj) == EdgeType::BACK && dfs.pre(v) < dfs.pre(w)) {
			// v has been discovered BEFORE w, this DF-Tree BACK edge must be reverted
			std::swap(v, w);
			DEBUG("    correcting to (%2s -> %2s)\n", GA.label(v).c_str(), GA.label(w).c_str());
			adj = e->adjTarget();
		}

		int val = 2 * G.numberOfNodes() + 1;
		if (dfs.edgeType(adj) == EdgeType::BACK) {
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

	for (auto e : edges) {
		auto adj = e.second;
		auto v = adj->theNode();
		auto w = adj->twinNode();


		adjs_out[v->index()].push_back(adj);
		adjs_all[v->index()].push_back(adj);

		adjs_in[w->index()].push_back(adj);
		adjs_all[w->index()].push_back(adj);
	}

	return edges;
}

std::vector<std::list<adjEntry>*> AlgorithmHopcroftTarjan::pathfinder(node v) {
	auto paths = std::vector<std::list<adjEntry>*>();
	auto path = new std::list<adjEntry>();
	pathfinder(v, path, paths);
	return paths;
}

void AlgorithmHopcroftTarjan::pathfinder(node v, std::list<adjEntry> *&path, std::vector<std::list<adjEntry>*>& paths) {
	DEBUG("processing node %2s\n", GA.label(v).c_str());
	for (auto w_adj : adjs_out[v->index()]) {
		edgeOfCycle(w_adj, path);
		auto w = GraphUtils::getTargetNodeFor(v, w_adj);

		if (dfs.edgeType(w_adj) == EdgeType::TREE) {
			DEBUG("  adding TREE edge %2s (%2s -> %2s)\n", GA.label(w_adj->theEdge()).c_str(),
				GA.label(v).c_str(), GA.label(w).c_str());

			path->push_back(w_adj);
			pathfinder(w, path, paths);

		} else {
			// BACK or FORWARD edges
			DEBUG("  adding BACK/FWD edge %2s (%2s -> %2s)\n", GA.label(w_adj->theEdge()).c_str(),
				GA.label(v).c_str(), GA.label(w).c_str());

			path->push_back(w_adj);
			paths.push_back(path);

			DEBUG("    starting new path\n");
			path = new std::list<adjEntry>();

		}
	}
}

bool AlgorithmHopcroftTarjan::strongly_planar(adjEntry adj, std::list<int>& attachments) {
	static auto iter = 0;
	if (iter >= 5) return false;
	iter++;

	auto blocks = std::stack<Block>();
	auto cycle = edgeOfCycle(adj);

	auto it = cycle->rbegin();
	auto ite = cycle->rend();
	// while ((*it)->theEdge()->index() != adj->theEdge()->index())
	// 	it++;

	DEBUG("strongly_planar cycle of %2s\n", GA.label(adj->theEdge()).c_str());
	for (; it != ite; it++) {
		auto cycle_adj = *it;

		// 21
		auto w = cycle_adj->theNode();
		DEBUG("  validating edges of node %2s (from edge to %2s)\n",
			GA.label(w).c_str(), GA.label(cycle_adj->twinNode()).c_str());
		
		if (w->index() == adj->theNode()->index()) {
			DEBUG("    reached original source, breaking\n");
			break;
		}

		auto first = true;
		for (auto adj : adjs_out[w->index()]) {
		//for(adj = w->firstAdj(); adj; adj = adj->succ()) {
			auto src = adj->theNode();
			auto x = adj->twinNode();
			DEBUG("    validating edge %2s: %2s -> %2s\n", GA.label(adj->theEdge()).c_str(),
				GA.label(src).c_str(), GA.label(x).c_str());

			if (first) {
				first = false;
				DEBUG("      skipping first!\n");
				continue;
			}
			
			// 22
			auto A = std::list<int>();
			if (dfs.edgeType(adj) == EdgeType::TREE) {
				DEBUG("      this is TREE edge\n");
				if (!strongly_planar(adj, A))
					return false;

			} else {
				DEBUG("      this is BACK edge\n");
				A.push_back(dfs.pre(x));

			}

			// 23
			auto b = Block(adj, A);
			while (true) {
				if (b.interlacesLeft(blocks))
					blocks.top().flip();
				
				if (b.interlacesLeft(blocks))
					return false;
				
				if (b.interlacesRight(blocks)) {
					b.combine(blocks.top());
					blocks.pop();

				} else break;
			}
			blocks.push(b);
		}

		// 24.

		// 25.
		attachments.clear();
	}

	return true;
}

bool AlgorithmHopcroftTarjan::isPlanar() {
	DEBUG_EXPR(GA.directed());
	auto dfsForest = dfs.nodes_pre;
	auto edges = orderedEdges();
	cycles = pathfinder(dfs.nodes_pre[0]);
	
	DEBUG("\nDF-Tree:\n");
	for (auto v : dfsForest) {
		auto pre = dfs.pre(v);
		auto post = dfs.post(v);
		auto lowpt1 = dfs.low1(v);
		auto lowpt2 = dfs.low2(v);
		DEBUG("    %2s (%2d) = %3d, %3d, L1: %3d, L2: %3d\n",
			GA.label(v).c_str(), v->index(), pre, post, lowpt1, lowpt2);
	}
	
	DEBUG("\nBi-connected components:\n");
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

	if (dfs.biconnected.size() > 1) {
		DEBUG("  Splitting components into subgraphs:\n");
		std::vector<std::pair<Graph, GraphAttributes>> subgraphs;
		for (auto component : dfs.biconnected) {
			if (component.size() <= 3) {
				DEBUG("    Component of size <=3 always planar!\n");
				continue;
			}

			List<node> vertices;
			for (auto vertex : component)
				vertices.pushBack(vertex);

			auto CG = Graph();
			inducedSubGraph(G, vertices.begin(), CG);
			auto CGA = GraphAttributes(CG, GraphAttributes::edgeLabel | GraphAttributes::nodeLabel);

			DEBUG("    Created subgraph (%d nodes, %d edges):\n",
				CG.numberOfNodes(), CG.numberOfEdges());
			for (auto edge : CG.edges) {
				auto adj = edge->adjSource();
				auto u = adj->theNode();
				auto v = adj->twinNode();

				DEBUG("      %2d: %2d -> %2d\n",
					edge->index(), u->index(), v->index());
			}

			DEBUG("    {\n");
			auto algo = AlgorithmHopcroftTarjan(CG, CGA);
			auto algoResult = algo.isPlanar();
			DEBUG("    }\n");
			if (!algoResult) {
				DEBUG("      Subgraph not planar!\n");
				return false;
			} else {
				DEBUG("      Subgraph planar! :)\n");
			}
		}

		return true;
	}

	DEBUG("\nOrdered edges:\n");
	for (auto entry : edges) {
		auto w_adj = entry.second;
		auto v = w_adj->theNode();
		auto w = w_adj->twinNode();
		
		DEBUG("  %2d: %2s (%2d) -> %2s (%2d)  %s\n", entry.first,
			GA.label(v).c_str(), v->index(),
			GA.label(w).c_str(), w->index(),
			dfs.edgeType(w_adj) == EdgeType::BACK ? "IS BACK" : "");
	}

	DEBUG("\nPaths:\n");
	DEBUG_EXPR(cycles.size());
	for (auto path : cycles) {
		//DEBUG("  ");
		//DEBUG_EXPR(path.size());
		auto first_adj = true;
		for (auto adj : *path) {
			auto v = adj->theNode();
			auto w = adj->twinNode();
			
			if (first_adj) {
				DEBUG("  %2s (%2d, pre %2d) -> %2s (%2d, pre %2d)",
					GA.label(v).c_str(), v->index(), dfs.pre(v),
					GA.label(w).c_str(), w->index(), dfs.pre(w));
				first_adj = false;

			} else {
				DEBUG(" -> %2s (%2d, pre %2d)",
					GA.label(w).c_str(), w->index(), dfs.pre(w));

			}
		}

		DEBUG("\n");
	}

	auto attachments = std::list<int>();
	auto result = false;
	try {
		result = strongly_planar(adjs_out[dfs.nodes_pre[0]->index()][0], attachments);

	} catch (char const *e) {
		printf("ERROR IN strongly_planar: %s\n", e);
		return false;
	}

	if (G.numberOfEdges() > 3 * G.numberOfNodes() - 3
			|| (G.numberOfNodes() >= 3 && G.numberOfEdges() > 3 * G.numberOfNodes() - 6)) {
		DEBUG("PLANARITY PRECONDITION FAILED!");
		return false;
	}

	return result;
}
