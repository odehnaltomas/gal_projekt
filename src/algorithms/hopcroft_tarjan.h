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
	Block(adjEntry e, std::list<int>& atts) {
		LeftSegments.push_back(e);
		LeftAttachments.insert(LeftAttachments.end(), atts.begin(), atts.end());
	}

	void flip() {
		std::swap(LeftAttachments, RightAttachments);
		std::swap(LeftSegments, RightSegments);
	}
	void combine(Block b) {
		LeftAttachments.insert(LeftAttachments.end(), b.LeftAttachments.begin(), b.LeftAttachments.end());
		RightAttachments.insert(RightAttachments.end(), b.RightAttachments.begin(), b.RightAttachments.end());
		LeftSegments.insert(LeftSegments.end(), b.LeftSegments.begin(), b.LeftSegments.end());
		RightSegments.insert(RightSegments.end(), b.RightSegments.begin(), b.RightSegments.end());
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

	bool interlacesLeft(std::stack<Block>& blocks) {
		//if (emptyLeftAtt()) throw "Left Attachment list is never supposed to be empty!";
		if (blocks.empty() || blocks.top().emptyLeftAtt()
				|| LeftAttachments.back() >= blocks.top().headLeftAtt())
			return false;
		
		return true;
	}
	bool interlacesRight(std::stack<Block>& blocks) {
		//if (emptyLeftAtt()) throw "Left Attachment list is never supposed to be empty!";
		if (blocks.empty() || blocks.top().emptyRightAtt()
				|| RightAttachments.back() >= blocks.top().headRightAtt())
			return false;
		
		return true;
	}

	void addToAttachments(std::list<int> attachments, int node_id) {
		if (!emptyRightAtt() && headRightAtt() > node_id) flip();
		attachments.insert(attachments.begin(), LeftAttachments.begin(), LeftAttachments.end());
		attachments.insert(attachments.begin(), RightAttachments.begin(), RightAttachments.end());
	}

private:
	std::list<int> LeftAttachments;
	std::list<int> RightAttachments;

	std::vector<adjEntry> LeftSegments;
	std::vector<adjEntry> RightSegments;
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

	bool strongly_planar(adjEntry adj, std::list<int>& attachments);

	std::vector<std::pair<int, adjEntry>> orderedEdges();
};
