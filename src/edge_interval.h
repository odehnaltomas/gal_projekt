#pragma once
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/GraphAttributes.h>

using namespace ogdf;


namespace EdgeInt {
    class EdgeInterval {
    public:
        EdgeInterval(edge lowReturnEdge, edge highReturnEdge);
        static EdgeInterval getEmptyInterval();
        void setLowReturnEdge(edge newLowReturnEdge);
        void setHighReturnEdge(edge newHighReturnEdge);
        edge getLowReturnEdge();
        edge getHighReturnEdge();
        bool isEmpty();
        string toString(GraphAttributes& GA);

    private:
        edge lowReturnEdge;
        edge highReturnEdge;
    };
}