#include "edge_interval.h"


namespace EdgeInt {
    EdgeInterval::EdgeInterval(edge lowReturnEdge, edge highReturnEdge):
    lowReturnEdge(lowReturnEdge), highReturnEdge(highReturnEdge) {}

    EdgeInterval EdgeInterval::getEmptyInterval() {
        return EdgeInterval(nullptr, nullptr);
    }

    void EdgeInterval::setLowReturnEdge(edge newLowReturnEdge) {
        this->lowReturnEdge = newLowReturnEdge;
    }

    void EdgeInterval::setHighReturnEdge(edge newHighReturnEdge) {
        this->highReturnEdge = newHighReturnEdge;
    }

    edge EdgeInterval::getLowReturnEdge() {
        return this->lowReturnEdge;
    }

    edge EdgeInterval::getHighReturnEdge() {
        return this->highReturnEdge;
    }

    bool EdgeInterval::isEmpty() {
        return ((this->lowReturnEdge == nullptr) && (this->highReturnEdge == nullptr));
    }

    string EdgeInterval::toString(GraphAttributes& GA) {
        string str = "low: ";
        if (this->lowReturnEdge == nullptr) {
            str.append("NULL");
        } else {
            str.append(GA.label(this->lowReturnEdge));
        }
        str.append(", high: ");
        if(this->highReturnEdge == nullptr) {
            str.append("NULL");
        } else {
            str.append(GA.label(this->highReturnEdge));
        }
        return str;
    }
}