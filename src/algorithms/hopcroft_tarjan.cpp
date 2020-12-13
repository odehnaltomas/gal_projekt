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

		auto v_index = v->index();
		auto w_index = w->index();

		adjs_out[v_index].push_back(adj);
		adjs_all[v_index].push_back(adj);

		adjs_in[w_index].push_back(adj);
		adjs_all[w_index].push_back(adj);
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
	DEBUG("processing node %2d\n", v->index());
	for (auto w_adj : adjs_out[v->index()]) {
		edgeOfCycle(w_adj, path);
		auto w = GraphUtils::getTargetNodeFor(v, w_adj);

		if (dfs.edgeType(w_adj) == EdgeType::TREE) {
			DEBUG("  adding TREE edge %2d (%2d -> %2d)\n", w_adj->theEdge()->index(),
				v->index(), w->index());

			path->push_back(w_adj);
			pathfinder(w, path, paths);

		} else {
			// BACK or FORWARD edges
			DEBUG("  adding BACK/FWD edge %2d (%2d -> %2d)\n", w_adj->theEdge()->index(),
				v->index(), w->index());

			path->push_back(w_adj);
			paths.push_back(path);

			DEBUG("    starting new path\n");
			path = new std::list<adjEntry>();

		}
	}
}

bool AlgorithmHopcroftTarjan::strongly_planar(adjEntry adj, std::list<int>& attachments) {
	// static auto iter = 0;
	// if (iter >= 5) return false;
	// iter++;

	auto blocks = std::stack<Block>();
	auto cycle = edgeOfCycle(adj);

	auto it = cycle->rbegin();
	auto ite = cycle->rend();
	// while ((*it)->theEdge()->index() != adj->theEdge()->index())
	// 	it++;

	DEBUG("strongly_planar cycle of edge %2d\n", adj->theEdge()->index());
	for (; it != ite; it++) {
		auto cycle_adj = *it;

		// 21
		auto w = cycle_adj->theNode();
		DEBUG("  validating edges of node %2d [pre %2d] (from edge to node %2d [pre %2d])\n",
			w->index(), dfs.pre(w),
			cycle_adj->twinNode()->index(), dfs.pre(cycle_adj->twinNode()));
		
		if (w->index() == adj->theNode()->index()) {
			DEBUG("    reached original source, breaking\n");
			break;
		}

		auto first = true;
		for (auto cycle_subadj : adjs_out[w->index()]) {
		//for(adj = w->firstAdj(); adj; adj = adj->succ()) {
			auto x = cycle_subadj->twinNode();
#ifdef DEBUG_ENABLED
			auto src = cycle_subadj->theNode();
			DEBUG("    validating edge %2d (%2d [pre %2d] -> %2d [pre %2d])\n",
				cycle_subadj->theEdge()->index(),
				src->index(), dfs.pre(src),
				x->index(), dfs.pre(x));
#endif

			if (first) {
				first = false;
				DEBUG("      skipping first!\n");
				continue;
			}
			
			// 22
			auto A = std::list<int>();
			if (dfs.edgeType(cycle_subadj) == EdgeType::TREE) {
				DEBUG("      this is TREE edge, validating recursively\n");
				DEBUG("      {\n");
				if (!strongly_planar(cycle_subadj, A))
					return false;
				DEBUG("      }\n");

			} else {
				DEBUG("      this is BACK edge\n");
				A.push_back(dfs.pre(x));

			}

			DEBUG("      current recursive attachments\n      ");
			DEBUG_EXPR(A.size());
#ifdef DEBUG_ENABLED
			for (auto att : A)
				DEBUG("        - %2d\n", att);
#endif

			//DEBUG("      updating attachments\n");

			// 23
			DEBUG("      validating attachments - edge(s) added\n");
			auto b = Block(A);
			while (true) {
				if (b.interlacesLeft(blocks)) {
					DEBUG("        interlace left detected, flipping sides!\n");
					blocks.top().flip();
				}
				
				if (b.interlacesLeft(blocks)) {
					DEBUG("        interlace left still detected - NOT planar!\n");
					return false;
				}
				
				if (b.interlacesRight(blocks)) {
					DEBUG("        interlace right detected, merging blocks!\n");
					b.combine(blocks.top());
					blocks.pop();

				} else break;
			}
			blocks.push(b);
		}

		// 24.
		while (!blocks.empty() && blocks.top().clean(dfs.pre(dfs.parent(w))))
			blocks.pop();
	}

	auto back_edge_target = cycle->back()->twinNode();

	// 25.
	DEBUG("  processing block stack\n    ");
	DEBUG_EXPR(blocks.size());

	attachments.clear();
	while (!blocks.empty()) {
		auto b = blocks.top();
		blocks.pop();

		if (!b.emptyLeftAtt() && !b.emptyRightAtt()
				&& b.headLeftAtt() > dfs.pre(back_edge_target)
				&& b.headRightAtt() > dfs.pre(back_edge_target)) {
			DEBUG("    block attachment check failed\n");
			DEBUG("      "); DEBUG_EXPR(dfs.pre(back_edge_target));
			DEBUG("        "); DEBUG_EXPR(b.headLeftAtt());
			DEBUG("        "); DEBUG_EXPR(b.headRightAtt());
			return false;

		}

		DEBUG("    adding block attachments\n");
		b.addToAttachments(attachments, dfs.pre(back_edge_target));
	}

	if (back_edge_target->index() != cycle->front()->theNode()->index()) {
		DEBUG("  adding cycle's back edge target node to attachments\n");
		attachments.push_back(dfs.pre(back_edge_target));
	}

	attachments.sort(std::greater<>());
	attachments.unique();
	DEBUG("  current attachments\n    ");
	DEBUG_EXPR(attachments.size());
#ifdef DEBUG_ENABLED
	for (auto att : attachments)
		DEBUG("    - %2d\n", att);
#endif

	return true;
}

///
/// source: http://www.cs.uoi.gr/~loukas/courses/grad/Algorithmic_Graph_Theory/index.files/Planarity%20testing.pdf
/// page 563
///
bool AlgorithmHopcroftTarjan::pathfinder_alt(int v) {
	auto _v = dfs.preToNode(v);
	DEBUG("pathfinder(%2d)\n  ", v);
	DEBUG_EXPR(adjs_out[_v->index()].size());

	// for w in A(v) do
	for (auto vw_adj : adjs_out[_v->index()]) {
		auto _w = vw_adj->twinNode();
		auto w = dfs.pre(_w);
		DEBUG("  %2d -> %2d (%2d -> %2d)\n", v, w, _v->index(), _w->index());

		// if v -> w then begin
		if (dfs.edgeType(vw_adj) == EdgeType::TREE) {
			DEBUG("    is TREE edge\n");

			// if s = 0 then begin
			if (s == 0) {
				// s := v;
				s = v;
				// p := p + 1;
				p = p + 1;
			} // end;

			// PATH(w) := p;
			path[w] = p;
			// PATHFINDER(w);
			if (!pathfinder_alt(w))
				return false;
			DEBUG("    sub-segment OK\n");

			// delete stack entries and blocks corresponding to vertices no smaller than v;
			// while (x, y) on B has ((STACK(x) >= v) or (x = 0)) and ((STACK(y) >= v) or (y = 0))
			while (!B.empty()) {
				auto x = B.top().first;
				auto y = B.top().second;

				// demorgan - (STACK(x) >= v) or (x = 0)
				if (stack[x] < v && x != 0)
					break;

				// demorgan - (STACK(y) >= v) or (y = 0)
				if (stack[y] < v && y != 0)
					break;
				
				// delete (x ,y) from B
				B.pop();
			}

			int x, y;
			// if (x, y) on B has STACK(x) >= v then
			if (!B.empty() && (x = B.top().first) >= v)
				//  replace (x, y) on B by (0, y)
				B.top().first = 0;

			// if (x, y) on B has STACK(y) >= v then replace (x, y) on B by (x, 0)
			if (!B.empty() && (y = B.top().second) >= v)
				//  replace (x, y) on B by (0, y)
				B.top().second = 0;
			
			// while NEXT(-1) != 0 and STACK(NEXT(-1)) >= v
			while (next[-1] != 0 && stack[next[-1]] >= v)
				// do NEXT(-1) := NEXT(NEXT(-1));
				next[-1] = next[next[-1]];
			
			// while NEXT(0) != 0 and STACK(NEXT(0)) >= v
			while (next[0] != 0 && stack[next[0]] >= v)
				// do NEXT(0) := NEXT(NEXT(0));
				next[0] = next[next[0]];
			
			// if PATH(w) != PATH(v) then begin
			if (path[w] != path[v]) {
				// all of segment with first edge (v, w) has now been embedded.
				// new blocks must be moved from right to left;

				// L' := 0;
				auto L_ = 0;

				// while (x, y) on B has (STACK(x) > f(PATH(w))) or (STACK(y) > f(PATH(w)))
				//   and () do begin
				while (!B.empty()) {
					auto x = B.top().first;
					auto y = B.top().second;

					// demorgan - (STACK(x) > f(PATH(w))) or (STACK(y) > f(PATH(w)))
					if (stack[x] <= f[path[w]] && stack[y] <= f[path[w]])
						break;
					
					// demorgan - STACK(NEXT(-1) != 0)
					if (stack[next[-1]] == 0)
						break;
					
					// if STACK(x) > f(PATH(w)) then begin
					if (stack[x] > f[path[w]]) {
						// if STACK(y) > f(PATH(w)) then go to nonplanar;
						if (stack[y] > f[path[w]])
							return false;
						
						// L' := x;
						L_ = x;

					} else { // end else begin comment STACK(y) > f(PATH(w));
						// SAVE := NEXT(L');
						// NEXT(L') := NEXT(-1);
						// NEXT(-1) := NEXT(y);
						// NEXT(y) := SAVE;
						std::swap(next[L_], next[y]);
						std::swap(next[L_], next[-1]);

						// L' := y;
						L_ = y;

					} // end;

					// delete (x, y) from B;
					B.pop();

				} // end;

				// block on B must be combined with new blocks just deleted;

				// delete (x, y) from B;
				B.pop();

				// if x != 0 then
				if (x != 0)
					// add (x, y) to B
					B.push({x, y});
				
				// else if (L' != 0) or (y != 0) then
				else if (L_ != 0 || y != 0)
					// add (L', y) to B;
					B.push({L_, y});
				
				// comment delete end-of-stack marker on right stack;
				// NEXT(-1) := NEXT(NEXT(-1));
				next[-1] = next[next[-1]];

			} // end;

		} else {
			// BACK edge, current path is complete
			// path is normal if f(PATH(s)) < w;
			DEBUG("    is BACK edge\n");

			// if s = 0 then begin
			if (s == 0) {
				// p := p + 1;
				p = p + 1;
				// s := v;
				s = v;
			} // end;

			// f(p) := w;
			f[p] = w;

			// switch blocks of entries from left to right so that p may be embedded on left;
			auto L_ = 0;
			auto R_ = -1;
			while (
				// (NEXT(L') != 0) and (STACK(NEXT(L')) > w)
				(next[L_] != 0 && stack[next[L_]] > w)
				// or
				||
				// (NEXT(R') != 0) and (STACK(NEXT(R')) > w)
				(next[R_] != 0 && stack[next[R_]] > w)
			) { // do begin
				int x, y;
				// if (x, y) on B has (x != 0) and (y != 0) then begin
				if (!B.empty() && (x = B.top().first) != 0 && (y = B.top().second) != 0) {
					// if STACK(NEXT(L_)) > w then begin
					if (stack[next[L_]] > w) {
						if (stack[next[R_]] > w)
							return false;

						// SAVE := NEXT(R');
						// NEXT(R') := NEXT(L');
						// SAVE(L') := SAVE;
						std::swap(next[L_], next[R_]);

						// SAVE := NEXT(x);
						// NEXT(x) := NEXT(y);
						// SAVE(y) := SAVE;
						std::swap(next[x], next[y]);

						// L' := y;
						L_ = y;
						// R' := x;
						R_ = x;

					// end else begin
					} else { // end else begin comment STACK(NEXT(R')) > w;
						// L' := x;
						L_ = y;
						// R' := y;
						R_ = x;

					} // end;

				// end else if (x, y) on B has x != 0 then begin comment STACK(NEXT(L')) > w;
				} else if (!B.empty() && (x = B.top().first)) {
					// SAVE := NEXT(x);
					// NEXT(x) := NEXT(R');
					// NEXT(R') := NEXT(L');
					// NEXT(L') := SAVE;
					std::swap(next[x], next[L_]);
					std::swap(next[x], next[R_]);

					// R' := x;
					R_ = x;

				// else if (x, y) on B has y != 0 then
				} else if (!B.empty() && B.top().second != 0)
					// R' = y;
					R_ = y;

				// delete (x, y) from B;
				B.pop();

			} // end;

			// comment add P to left stack if p is normal;
			// if f(PATH(s)) < w then begin
			if (f[path[s]] < w) {
				// if L' = 0 then
				if (L_ == 0)
					// L' := FREE;
					L_ = free;
				
				// STACK(FREE) := f;
				stack[free] = f[path[s]]; //?
				// NEXT(FREE) := NEXT(0);
				next[free] = next[0];
				// NEXT(0) := FREE;
				next[0] = free;
				// FREE := FREE + 1;
				free++;

			} // end;

			// add new block corresponding to combined old blocks.
			// new block may be empty if segment containing current path is not a single frond;

			// if R' = -1 then
			if (R_ == -1)
				// R' := 0;
				R_ = 0;
			
			// if (L' != 0) or (R' != 0) or (v != s) then
			if (L_ != 0 || R_ != 0 || v != s)
				// add (L', R') to B;
				B.push({L_, R_});
			
			// if segment containing current path is not a single frond,
			// add an end of stack marker to right stack;

			// if v != s then begin
			if (v != s) {
				// STACK(FREE) := 0;
				stack[free] = 0;
				// NEXT(FREE) := NEXT(-1);
				next[free] = next[-1];
				// NEXT(-1) := FREE;
				next[-1] = free;
				// FREE := FREE + 1;
				free++;
				

			} // end;

			// s := 0;
			s = 0;

 		} // end;

	} // end;

	return true;
}

///
/// source: http://www.cs.uoi.gr/~loukas/courses/grad/Algorithmic_Graph_Theory/index.files/Planarity%20testing.pdf
/// page 563
///
bool AlgorithmHopcroftTarjan::embed() {
	// NEXT(-1) := NEXT(0) := 0;
	next[-1] = next[0] = 0;

	// FREE := 1;
	free = 1;

	// STACK(0) := 0;
	stack[0] = 0;

	// B := the empty stack;
	// already initialized

	// p := s := 0;
	p = s = 0;

	// PATH(1) := 1;
	auto v_first = 0;
	path[v_first] = 1;

	return pathfinder_alt(v_first);
}

bool AlgorithmHopcroftTarjan::isPlanar() {
//	if (G.numberOfEdges() > 3 * G.numberOfNodes() - 3
//			|| (G.numberOfNodes() >= 3 && G.numberOfEdges() > 3 * G.numberOfNodes() - 6)) {
//		DEBUG("PLANARITY PRECONDITION FAILED!\n");
//		return false;
//	}

	auto dfsForest = dfs.nodes_pre;
	auto edges = orderedEdges();
	cycles = pathfinder(dfs.nodes_pre[0]);
	
#ifdef DEBUG_ENABLED
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
#endif

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

#ifdef DEBUG_ENABLED
			for (auto edge : CG.edges) {
				auto adj = edge->adjSource();
				auto u = adj->theNode();
				auto v = adj->twinNode();

				DEBUG("      %2d: %2d -> %2d\n",
					edge->index(), u->index(), v->index());
			}
#endif

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
	
#ifdef DEBUG_ENABLED
	DEBUG("\nDF-Tree:\n");
	for (auto v : dfsForest) {
		auto pre = dfs.pre(v);
		auto post = dfs.post(v);
		auto lowpt1 = dfs.low1(v);
		auto lowpt2 = dfs.low2(v);
		DEBUG("    %2s (%2d) = %3d, %3d, L1: %3d, L2: %3d\n",
			GA.label(v).c_str(), v->index(), pre, post, lowpt1, lowpt2);
	}
#endif

#ifdef DEBUG_ENABLED
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
#endif

#ifdef DEBUG_ENABLED
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
#endif

	auto attachments = std::list<int>();
	auto result = false;
	try {
		result = strongly_planar(adjs_out[dfs.nodes_pre[0]->index()][0], attachments);

	} catch (char const *e) {
		DEBUG("ERROR IN strongly_planar: %s\n", e);
		return false;
	}

//#ifdef DEBUG_ENABLED
//	auto result_alt = embed();
//	DEBUG("result: %d\nresult alt: %d\n", result, result_alt);
//#endif
	return result;
}
