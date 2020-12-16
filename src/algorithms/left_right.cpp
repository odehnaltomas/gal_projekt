#include <limits.h>
#include <algorithm>
#include <iostream>

#include "left_right.h"
#include "counting_sort.h"

 
using namespace ogdf;

AlgorithmLeftRight::AlgorithmLeftRight(Graph& g, GraphAttributes& ga): G(g), GA(ga) {
    this->roots = std::vector<node>();
    this->height_max = std::numeric_limits<int>::max();
    this->height = NodeArray<int>(this->G, height_max);
    this->nestingDepth = EdgeArray<int>(this->G, -1);
    this->lowopt = EdgeArray<int>(this->G);
    this->lowopt2 = EdgeArray<int>(this->G);
    this->parentEdgeArr = NodeArray<edge>(this->G, nullptr);
    this->oriented = EdgeArray<bool>(this->G, false);
    this->allOriented = 0;
    this->nodeOutEdges = NodeArray<std::vector<edge>>(this->G, std::vector<edge>());

    this->stackBottom = EdgeArray<unsigned int>(this->G);
    this->side = EdgeArray<int>(this->G, 1);
    this->ref = EdgeArray<edge>(this->G, nullptr);
    this->lowopt_edge = EdgeArray<edge>(this->G);
}

bool AlgorithmLeftRight::isPlanar() {
    if (this->G.numberOfNodes() <= 2) {
        // A graph with at most 2 nodes (vertices) is planar
        return true;
    }

    if (this->G.numberOfEdges() > (3 * this->G.numberOfNodes() - 6)) {
        // Planar graph can have at most (3 * N - 6), N - number of nodes
        std::cout << "wow" << std::endl;
        return false;
    }

    /** Orientation part */
    for (node s : this->G.nodes) {
        if (this->height[s] == this->height_max) {
            this->roots.push_back(s);
            this->height[s] = 0;
            this->doDFS1(s);
//            std::cout << "Node: " << this->GA.label(s) << std::endl;
//            if (this->parentEdge[s] == nullptr) {
//                std::cout << "jojo" << std::endl;
//            }
        }
    }
//    std::cout << "----AFTER ORIENTATION----" << std::endl << "Nodes height: " << std::endl;
//    for (node s : this->G.nodes) {
//        std::cout << "\tnode id: " << this->GA.label(s) << " = " << this->height[s] << std::endl;
//    }
//
//    std::cout << "Edges lowopt: " << std::endl;
//    for (edge e : this->G.edges) {
//        std::cout << "\tedge id: " << this->GA.label(e) << " = " << this->lowopt[e] << std::endl;
//    }
//
//    std::cout << "Edges lowopt2: " << std::endl;
//    for (edge e : this->G.edges) {
//        std::cout << "\tedge id: " << this->GA.label(e) << " = " << this->lowopt2[e] << std::endl;
//    }
//
//    std::cout << "Edges nesting_depth: " << std::endl;
//    for (edge e : this->G.edges) {
//        std::cout << "\tedge id: " << this->GA.label(e) << " = " << this->nestingDepth[e] << std::endl;
//    }
//
//    for (node n : G.nodes) {
//        std::cout << std::endl << std::endl << "NODE: " << GA.label(n);
//        for (edge e : nodeOutEdges[n]) {
//            std::cout << "\tedge id: " << GA.label(e) << ", nestingDepth: " << this->nestingDepth[e] << std::endl;
//        }
//    }
    /** Testing part */
    CntSort::CountingSort::sort(this->GA, this->G, this->nodeOutEdges, this->nestingDepth);
//    for (node n : G.nodes) {
//        std::cout << std::endl << std::endl << "NODE: " << GA.label(n);
//        for (edge e : nodeOutEdges[n]) {
//            std::cout << "\tedge id: " << GA.label(e) << ", nestingDepth: " << nestingDepth[e] << std::endl;
//            std::cout << "\t\tsource: " << GA.label(e->source()) << ", target: " << GA.label(e->target()) << std::endl;
//        }
//    }
    for (node s : this->roots) {
        bool result = this->doDFS2(s);
        if (result) {
            return true;
        } else {
//            std::cout << "Graph is not planar." << std::endl;
        }
    }
	return false;
}

void AlgorithmLeftRight::doDFS1(node v) {
    edge parentEdge = this->parentEdgeArr[v];
//    std::cout << "node: " << GA.label(v) << std::endl;
    for (adjEntry adj : v->adjEntries) {
        // If all edges are oriented stop
        if (this->allOriented == this->G.numberOfEdges()) {
            break;
        }

        edge e = adj->theEdge();
        if (this->oriented[e]) {
//            adjEntry source = e->adjSource();
//            std::cout << "\talready oriented edge: " << GA.label(e) << std::endl;
//            std::cout << "\t\tsource: " << GA.label(source->theNode()) << ", target: " << GA.label(e->target()) << std::endl;
            continue;
        }
        adjEntry source = e->adjSource();
//        std::cout << "\tnon-oriented edge: " << GA.label(e) << std::endl;
//        std::cout << "\t\tsource: " << GA.label(source->theNode()) << ", target: " << GA.label(e->target()) << std::endl;

        node w;
        if (source->theNode() == v) { /** Edge has right orientation */
            this->oriented[e] = true;
            this->allOriented++;
            w = e->target();

            this->nodeOutEdges[v].push_back(e);
        } else { /** Edge has wrong orientation must be tured (swap source and target nodes of edge) */
            //adjEntry target = e->adjTarget();
//            std::cout << "\t\tsource: " << GA.label(e->source()) << ", target: " << GA.label(e->target()) << std::endl << std::endl;
            this->G.reverseEdge(e);
//            std::cout << "\t-TO-"<< std::endl;
//            std::cout << "\toriented edge: " << GA.label(e) << std::endl;
//            std::cout << "\t\tsource: " << GA.label(e->source()) << ", target: " << GA.label(e->target()) << std::endl << std::endl;
            this->oriented[e] = true;
            this->allOriented++;
            w = e->target();

            this->nodeOutEdges[v].push_back(e);
        }

        this->lowopt[e] = this->height[v];
        this->lowopt2[e] = this->height[v];
        if (this->height[w] == this->height_max) { /** Tree edge */
//            std::cout << "\tedge: " << GA.label(e) << " is TREE EDGE" << std::endl;
            this->parentEdgeArr[w] = e;
            this->height[w] = this->height[v] + 1;
            this->doDFS1(w);
//            std::cout << "return to node: " << GA.label(v) << std::endl;
        } else { /** Back edge */
//            std::cout << "\tedge: " << GA.label(e) << " is BACKWARD EDGE" << std::endl;
            this->lowopt[e] = this->height[w];
        }

        /** Determine nesting depth */
        this->nestingDepth[e] = 2 * this->lowopt[e];
        if (this->lowopt2[e] < this->height[v]) {
            this->nestingDepth[e] += 1;
        }

        /** Update lowpoint of parent edge */
        if (parentEdge != nullptr) {
            if (this->lowopt[e] < this->lowopt[parentEdge]) {
                this->lowopt2[parentEdge] = std::min(this->lowopt[parentEdge], this->lowopt2[e]);
                this->lowopt[parentEdge] = this->lowopt[e];
            } else if (this->lowopt[e] > this->lowopt[parentEdge]) {
                this->lowopt2[parentEdge] = std::min(this->lowopt2[parentEdge], this->lowopt[e]);
            } else {
                this->lowopt2[parentEdge] = std::min(this->lowopt2[parentEdge], this->lowopt2[e]);
            }
        }
    }
}

bool AlgorithmLeftRight::doDFS2(node v) {
    edge parentEdge = this->parentEdgeArr[v];
//    std::cout << "START NODE: " << GA.label(v) << std::endl;
    for (edge e : this->nodeOutEdges[v]) {
//        std::cout << "start edge: " << GA.label(e) << std::endl;
//        std::cout << "stack size: " << this->S.size() << std::endl;
        this->stackBottom[e] = this->S.size();

        edge trg_par_edge = this->parentEdgeArr[e->target()];
//        std::cout << "pred porovnanim" << std::endl;
//        if (trg_par_edge != nullptr && (!EdgeElement::compare(*(trg_par_edge), *e))) { /** TREE EDGE */
        if (e == trg_par_edge) { /** TREE EDGE */
//            std::cout << "po porovnani - TREE EDGE" << std::endl;
            bool result = this->doDFS2(e->target());
//            std::cout << "BACK TO NODE: " << GA.label(v) << " with actual edge: " << GA.label(e) << std::endl;
            if (!result) {
                return false;
            }
        } else {
//            std::cout << "po porovnani - BACK EDGE" << std::endl;
            this->lowopt_edge[e] = e;
            EdgeInt::EdgeInterval fst_int = EdgeInt::EdgeInterval::getEmptyInterval();
            EdgeInt::EdgeInterval snd_int(e, e);
            ConfPair::ConflictPair conflictPair(fst_int, snd_int);
            this->S.push(ConfPair::ConflictPair(fst_int, snd_int));
//            std::cout << "Pushed " << conflictPair.toStr(this->GA) << " to STACK" << std::endl;
        }

//        std::cout << "lowopt hrany " << this->GA.label(e) << " je " << this->lowopt[e] << ", height uzlu " << this->GA.label(v) << " je " << this->height[v] << std::endl;
        if (this->lowopt[e] < this->height[v]) {
//            std::cout << "splnuje" << std::endl;
            if (e == this->nodeOutEdges[v].front()) {
//                std::cout << this->GA.label(e) << "je prvni" << std::endl;
                this->lowopt_edge[parentEdge] = this->lowopt_edge[e];
            } else {
//                std::cout << "\tstart constraints" << std::endl;
                bool result = addConstraints(e, parentEdge);
//                std::cout << "\tend constraints" << std::endl;
                if (!result) {
                    return false;
                }
            }
        }
    }

    if (parentEdge != nullptr) {
        node u = parentEdge->source();
//        std::cout << "\tstart trim" << std::endl;
        trimBackEdges(u);
//        std::cout << "\tend trim" << std::endl;

        if (this->lowopt[parentEdge] < this->height[u]) {
            edge hl = this->S.top().getLeftInterval().getHighReturnEdge();
            edge hr = this->S.top().getRightInterval().getHighReturnEdge();
            if ((hl != nullptr) && ((hr == nullptr) || (this->lowopt[hl] > this->lowopt[hr]))) {
                this->ref[parentEdge] = hl;
            } else {
                this->ref[parentEdge] = hr;
            }
        }
    }
    return true;
}

/**
 * This method do constraint integration. It is the most essential step of this algorithm.
 * @param e Edge to which added constraints are associated.
 * @param parentEdge
 * @return false if any condition is violated (graph is not planar), otherwise true (graph is planar).
 */
bool AlgorithmLeftRight::addConstraints(edge e, edge parentEdge) {
    showStack(this->S);
    ConfPair::ConflictPair P = ConfPair::ConflictPair::getEmptyConflictPair();
//    std::cout << "P = " << P.toStr(this->GA) << std::endl;
    while (this->S.size() > this->stackBottom[e]) {
        ConfPair::ConflictPair Q = this->S.top();
        this->S.pop();

//        std::cout << "Q = " << Q.toStr(this->GA) << std::endl;
        if (!Q.getLeftInterval().isEmpty() ) {
//            std::cout << "Q.L neni prazdny -> udelej swap" << std::endl;
            Q.swapIntervals();
//            std::cout << "Q = " << Q.toStr(this->GA) << std::endl;
        } else {
//            std::cout << "Q.L je prazdny -> zadny swap" << std::endl;
        }
        if (!Q.getLeftInterval().isEmpty() ) {
            return false;
        } else {
//            std::cout << "stale planarni" << std::endl;
//            std::cout << "lowopt[Q.R.low] = " << this->lowopt[Q.getRightInterval().getLowReturnEdge()] << ", lowopt[parentEdge] = " << this->lowopt[parentEdge] << std::endl;
            if (this->lowopt[Q.getRightInterval().getLowReturnEdge()] > this->lowopt[parentEdge]) { /** merge intervals */
                if (P.getRightInterval().isEmpty()) {
//                    std::cout << "P.R je prazdny" << std::endl;
                    P.getRightInterval().setHighReturnEdge(Q.getRightInterval().getHighReturnEdge());
//                    interval.setLowReturnEdge(nullptr);
//                    P.setRightInterval(interval);
//                    std::cout << "P = " << P.toStr(this->GA) << std::endl;
                } else {
                    this->ref[P.getRightInterval().getLowReturnEdge()] = Q.getRightInterval().getHighReturnEdge();
                }
                P.getRightInterval().setLowReturnEdge(Q.getRightInterval().getLowReturnEdge());
//                std::cout << "P = " << P.toStr(this->GA) << std::endl;
            } else { /** make consistent */
                this->ref[Q.getRightInterval().getLowReturnEdge()] = this->lowopt_edge[parentEdge];
            }
        }
    }

    showStack(this->S);
//    std::cout << "totok" << std::endl;
    while (conflicting(this->S.top().getLeftInterval(), e) || conflicting(this->S.top().getRightInterval(), e)) {
        ConfPair::ConflictPair Q = this->S.top();
        this->S.pop();

        if (conflicting(Q.getRightInterval(), e)) {
            Q.swapIntervals();
        }
        if (conflicting(Q.getRightInterval(), e)) {
            return false;
        } else { /** merge interval below lowpt(e) into P.R */
            this->ref[P.getRightInterval().getLowReturnEdge()] = Q.getRightInterval().getHighReturnEdge();
            if (Q.getRightInterval().getLowReturnEdge() != nullptr) {
                P.getRightInterval().setLowReturnEdge(Q.getRightInterval().getLowReturnEdge());
            }
        }

        if (P.getLeftInterval().isEmpty()) {
            P.getLeftInterval().setHighReturnEdge(Q.getLeftInterval().getHighReturnEdge());
        } else {
            this->ref[P.getLeftInterval().getLowReturnEdge()] = Q.getLeftInterval().getHighReturnEdge();
        }
        P.getLeftInterval().setLowReturnEdge(Q.getLeftInterval().getLowReturnEdge());
    }
    if (!P.isEmpty()) {
        this->S.push(P);
    }
    showStack(this->S);
    return true;
}

bool AlgorithmLeftRight::conflicting(EdgeInt::EdgeInterval interval, edge b) {
    return ((!interval.isEmpty()) && (this->lowopt[interval.getHighReturnEdge()] > this->lowopt[b]));
}

void AlgorithmLeftRight::trimBackEdges(node u){
    /** Drop entire conflict pairs */
    while ((!this->S.empty()) && (lowest(this->S.top()) == this->height[u])) {
        ConfPair::ConflictPair P = this->S.top();
        this->S.pop();

        if (P.getLeftInterval().getLowReturnEdge() != nullptr) {
            this->side[P.getLeftInterval().getLowReturnEdge()] = -1;
        }
    }

    /** One more conflict pair to consider */
    if (!this->S.empty()) {
        ConfPair::ConflictPair P = this->S.top();
        this->S.pop();
        /** Trime left interval BEGIN */
        while ((P.getLeftInterval().getHighReturnEdge() != nullptr) &&
               (P.getLeftInterval().getHighReturnEdge()->target() == u)) {
            P.getLeftInterval().setHighReturnEdge(this->ref[P.getLeftInterval().getHighReturnEdge()]);
        }
        if ((P.getLeftInterval().getHighReturnEdge() == nullptr) && (P.getLeftInterval().getLowReturnEdge() != nullptr)) {
            /** Just emptied */
            this->ref[P.getLeftInterval().getLowReturnEdge()] = P.getRightInterval().getLowReturnEdge();
            this->side[P.getLeftInterval().getLowReturnEdge()] = -1;
            P.getLeftInterval().setLowReturnEdge(nullptr);
        }
        /** Trime left interval END */

        /** Trim right interval BEGIN*/
        while ((P.getRightInterval().getHighReturnEdge() != nullptr) &&
               (P.getRightInterval().getHighReturnEdge()->target() == u)) {
            P.getRightInterval().setHighReturnEdge(this->ref[P.getRightInterval().getHighReturnEdge()]);
        }
        if ((P.getRightInterval().getHighReturnEdge() == nullptr) && (P.getRightInterval().getLowReturnEdge() != nullptr)) {
            /** Just emptied */
            this->ref[P.getRightInterval().getLowReturnEdge()] = P.getLeftInterval().getLowReturnEdge();
            this->side[P.getRightInterval().getLowReturnEdge()] = -1;
            P.getRightInterval().setLowReturnEdge(nullptr);
        }
        /** Trim right interval END*/
        this->S.push(P);
    }
}

int AlgorithmLeftRight::lowest(ConfPair::ConflictPair &P) {
    if (P.getLeftInterval().isEmpty()) {
        return this->lowopt[P.getRightInterval().getLowReturnEdge()];
    }
    if (P.getRightInterval().isEmpty()) {
        return this->lowopt[P.getLeftInterval().getLowReturnEdge()];
    }
    return std::min(this->lowopt[P.getLeftInterval().getLowReturnEdge()], this->lowopt[P.getRightInterval().getLowReturnEdge()]);
}

void AlgorithmLeftRight::showStack(std::stack<ConfPair::ConflictPair> s) {
//    std::cout << "------STACK------" << std::endl;
    while (!s.empty()) {
//        std::cout << s.top().toStr(this->GA);
        s.pop();
    }
//    std::cout << "------STACK END------" << std::endl;
}
