#pragma once
#include<list>
#include<vector>
#include<ogdf/basic/Graph_d.h>
#include<ogdf/basic/GraphAttributes.h>
#include "../utils.h"
 
using namespace ogdf;

///
/// source: https://people.mpi-inf.mpg.de/~mehlhorn/ftp/ImplementationPlanarity.pdf
///
class Block
{
public:
	Block(std::list<int>& atts) {
		LeftAttachments.insert(LeftAttachments.end(), atts.begin(), atts.end());
	}

	void flip() {
		std::swap(LeftAttachments, RightAttachments);
	}
	void combine(Block b) {
		LeftAttachments.insert(LeftAttachments.end(), b.LeftAttachments.begin(), b.LeftAttachments.end());
		LeftAttachments.sort(std::greater<>());

		RightAttachments.insert(RightAttachments.end(), b.RightAttachments.begin(), b.RightAttachments.end());
		RightAttachments.sort(std::greater<>());
	}
	bool clean(int node_id) {
		while (!emptyLeftAtt() && headLeftAtt() == node_id) LeftAttachments.pop_front();
		while (!emptyRightAtt() && headRightAtt() == node_id) RightAttachments.pop_front();
		return emptyLeftAtt() && emptyRightAtt();
	}

	int headLeftAtt() {
		if (emptyLeftAtt()) throw "Tried accessing head of Left Attachment list when empty.";
		return LeftAttachments.front();
	}
	int headRightAtt() {
		if (emptyRightAtt()) throw "Tried accessing head of Right Attachment list when empty.";
		return RightAttachments.front();
	}

	int emptyLeftAtt() { return LeftAttachments.empty(); }
	int emptyRightAtt() { return RightAttachments.empty(); }

	bool interlacesLeft(std::stack<Block> &blocks) {
		DEBUG("        interlacesLeft():\n");
		DEBUG("          "); DEBUG_EXPR(LeftAttachments.back());

		if (emptyLeftAtt()) throw "Left Attachment list is never supposed to be empty!";
		if (blocks.empty() || blocks.top().emptyLeftAtt()
				|| LeftAttachments.back() >= blocks.top().headLeftAtt()) {
			DEBUG("          NO interlace left detected\n");
			return false;
		}
		
		DEBUG("          "); DEBUG_EXPR(blocks.top().headLeftAtt());
		return true;
	}
	bool interlacesRight(std::stack<Block> &blocks) {
		DEBUG("        interlacesRight():\n");
		DEBUG("          "); DEBUG_EXPR(LeftAttachments.back());

		if (emptyLeftAtt()) throw "Left Attachment list is never supposed to be empty!";
		if (blocks.empty() || blocks.top().emptyRightAtt()
				|| RightAttachments.back() >= blocks.top().headRightAtt()) {
			DEBUG("          NO interlace right detected\n");
			return false;
		}
		
		DEBUG("          "); DEBUG_EXPR(blocks.top().headRightAtt());
		return true;
	}

	void addToAttachments(std::list<int> &attachments, int node_id) {
		if (!emptyRightAtt() && headRightAtt() > node_id) flip();
		attachments.insert(attachments.begin(), LeftAttachments.begin(), LeftAttachments.end());
		attachments.insert(attachments.begin(), RightAttachments.begin(), RightAttachments.end());
	}

private:
	std::list<int> LeftAttachments;
	std::list<int> RightAttachments;
};

class AlgorithmHopcroftTarjan
{
public:
	Graph G;
	GraphAttributes GA;
	DFS::DFS dfs;
	
    AlgorithmHopcroftTarjan(Graph& g, GraphAttributes& ga);

	bool isPlanar();

protected:
	std::map<int, std::vector<adjEntry>> adjs_all;
	std::map<int, std::vector<adjEntry>> adjs_in;
	std::map<int, std::vector<adjEntry>> adjs_out;

	std::vector<std::list<adjEntry>*> cycles;
	std::map<int, std::list<adjEntry>*> edge_cycle;

	std::list<adjEntry>* edgeOfCycle(adjEntry adj);
	void edgeOfCycle(adjEntry adj, std::list<adjEntry>* cycle);

	std::vector<std::list<adjEntry>*> pathfinder(node v);
	void pathfinder(node v, std::list<adjEntry> *&path, std::vector<std::list<adjEntry>*>& paths);

	std::map<int, int> next, stack, path, f;
	std::stack<std::pair<int, int>> B;

	int p, s, free;

	bool pathfinder_alt(int v);
	bool embed();

	bool strongly_planar(adjEntry adj, std::list<int>& attachments);

	std::vector<std::pair<int, adjEntry>> orderedEdges();
};
