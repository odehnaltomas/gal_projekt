#include "conflict_pair.h"


namespace ConfPair {
    ConflictPair::ConflictPair(EdgeInt::EdgeInterval &leftInterval, EdgeInt::EdgeInterval &rightInterval) :
            leftInterval(leftInterval), rightInterval(rightInterval) {}

    ConflictPair ConflictPair::getEmptyConflictPair() {
        EdgeInt::EdgeInterval fstInt = EdgeInt::EdgeInterval::getEmptyInterval();
        EdgeInt::EdgeInterval sndInt = EdgeInt::EdgeInterval::getEmptyInterval();
        return ConflictPair(fstInt, sndInt);
    }

    EdgeInt::EdgeInterval ConflictPair::getLeftInterval() {
        return this->leftInterval;
    }

    EdgeInt::EdgeInterval ConflictPair::getRightInterval() {
        return this->rightInterval;
    }

    void ConflictPair::setLeftInterval(EdgeInt::EdgeInterval interval) {
        this->leftInterval = interval;
    }

    void ConflictPair::setRightInterval(EdgeInt::EdgeInterval interval) {
        this->rightInterval = interval;
    }

//    bool ConflictPair::isIntervalEmpty(Interval interval) {
//        if(interval == Interval::Left) {
//            return this->leftInterval.isEmpty();
//        } else {
//            return this->rightInterval.isEmpty();
//        }
//    }

    void ConflictPair::swapIntervals() {
        EdgeInt::EdgeInterval tmp = this->leftInterval;
        this->leftInterval = this->rightInterval;
        this->rightInterval = tmp;
    }

    bool ConflictPair::isEmpty() {
        return (this->leftInterval.isEmpty() && this->rightInterval.isEmpty());
    }
}