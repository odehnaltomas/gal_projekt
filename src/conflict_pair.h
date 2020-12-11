#pragma once

#include "edge_interval.h"

using namespace ogdf;

namespace ConfPair {
    class ConflictPair {
    public:
        ConflictPair(EdgeInt::EdgeInterval &leftInterval, EdgeInt::EdgeInterval &rightInterval);
        static ConflictPair getEmptyConflictPair();
        EdgeInt::EdgeInterval getLeftInterval();
        EdgeInt::EdgeInterval getRightInterval();
        void setLeftInterval(EdgeInt::EdgeInterval interval);
        void setRightInterval(EdgeInt::EdgeInterval interval);
//        bool isIntervalEmpty(Interval);
        void swapIntervals();
        bool isEmpty();

    private:
        EdgeInt::EdgeInterval leftInterval;
        EdgeInt::EdgeInterval rightInterval;
    };
}