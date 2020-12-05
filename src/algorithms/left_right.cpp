#include <limits.h>
#include <algorithm>
#include <iostream>

#include "left_right.h"

 
using namespace ogdf;

AlgorithmLeftRight::AlgorithmLeftRight(Graph& g, GraphAttributes& ga): G(g), GA(ga) {
    this->height_max = std::numeric_limits<int>::max();
    this->height = NodeArray<int>(this->G, height_max);
    this->nestingDepth = EdgeArray<int>(this->G, -1);
    this->lowopt = EdgeArray<int>(this->G);
    this->lowopt2 = EdgeArray<int>(this->G);
    this->parentEdgeArr = NodeArray<edge>(this->G, nullptr);
    this->oriented = EdgeArray<bool>(this->G, false);
    this->allOriented = 0;
}

bool AlgorithmLeftRight::isPlanar() {
    if (this->G.numberOfNodes() <= 2) {
        // A graph with at most 2 nodes (vertices) is planar
        return true;
    }

    if (this->G.numberOfEdges() > (3 * this->G.numberOfNodes() - 6)) {
        // Planar graph can have at most (3 * N - 6), N - number of nodes
        return false;
    }

    // Orientation part
    for (node s : this->G.nodes) {
        if (this->height[s] == this->height_max) {
            this->height[s] = 0;
            this->doDFS1(s);
//            std::cout << "Node: " << this->GA.label(s) << std::endl;
//            if (this->parentEdge[s] == nullptr) {
//                std::cout << "jojo" << std::endl;
//            }
        }
    }
    std::cout << "----AFTER ORIENTATION----" << std::endl << "Nodes height: " << std::endl;
    for (node s : this->G.nodes) {
        std::cout << "\tnode id: " << this->GA.label(s) << " = " << this->height[s] << std::endl;
    }

    std::cout << "Edges lowopt: " << std::endl;
    for (edge e : this->G.edges) {
        std::cout << "\tedge id: " << this->GA.label(e) << " = " << this->lowopt[e] << std::endl;
    }

    std::cout << "Edges lowopt2: " << std::endl;
    for (edge e : this->G.edges) {
        std::cout << "\tedge id: " << this->GA.label(e) << " = " << this->lowopt2[e] << std::endl;
    }

    std::cout << "Edges nesting_depth: " << std::endl;
    for (edge e : this->G.edges) {
        std::cout << "\tedge id: " << this->GA.label(e) << " = " << this->nestingDepth[e] << std::endl;
    }
	return false;
}

void AlgorithmLeftRight::doDFS1(node v) {
    edge parentEdge = this->parentEdgeArr[v];
    std::cout << "node: " << GA.label(v) << std::endl;
    for (adjEntry adj : v->adjEdges()) {
        // If all edges are oriented stop
//        if (this->allOriented == this->G.numberOfEdges()) {
//            break;
//        }

        edge e = adj->theEdge();
        if (this->oriented[e]) {
            adjEntry source = e->adjSource();
            std::cout << "\talready oriented edge: " << GA.label(e) << std::endl;
            std::cout << "\t\tsource: " << GA.label(source->theNode()) << ", target: " << GA.label(e->target()) << std::endl;
            continue;
        }
        adjEntry source = e->adjSource();
        std::cout << "\tnon-oriented edge: " << GA.label(e) << std::endl;
        std::cout << "\t\tsource: " << GA.label(source->theNode()) << ", target: " << GA.label(e->target()) << std::endl;

        node w;
        if (source->theNode() == v) {
            this->oriented[e] = true;
            this->allOriented++;
            w = e->target();
        } else {
            //adjEntry target = e->adjTarget();
            std::cout << "\t\tsource: " << GA.label(e->source()) << ", target: " << GA.label(e->target()) << std::endl << std::endl;
            this->G.reverseEdge(e);
            std::cout << "\t-TO-"<< std::endl;
            std::cout << "\toriented edge: " << GA.label(e) << std::endl;
            std::cout << "\t\tsource: " << GA.label(e->source()) << ", target: " << GA.label(e->target()) << std::endl << std::endl;
            this->oriented[e] = true;
            this->allOriented++;
            w = e->target();
        }

        this->lowopt[e] = this->height[v];
        this->lowopt2[e] = this->height[v];
        if (this->height[w] == this->height_max) { /** Tree edge */
            std::cout << "\tedge: " << GA.label(e) << " is TREE EDGE" << std::endl;
            this->parentEdgeArr[w] = e;
            this->height[w] = this->height[v] + 1;
            this->doDFS1(w);
            std::cout << "return to node: " << GA.label(v) << std::endl;
        } else { /** Back edge */
            std::cout << "\tedge: " << GA.label(e) << " is BACKWARD EDGE" << std::endl;
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