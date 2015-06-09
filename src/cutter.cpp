#include "cutter.hpp"

#include <algorithm>

#include "string_helpers.hpp"

void Cutter::reset() {
    fragmentedSegmentsQueue_ = std::queue<AnnotationItem>();

    doReset();
}

size_t Cutter::segmentLengthHardLimit() {
    return doSegmentLengthHardLimit();
}

size_t Cutter::segmentLengthSoftLimit() {
    return doSegmentLengthSoftLimit();
}

Cutter::~Cutter() {
}

bool Cutter::shouldFragmentQueueBeUsed_() const {
    return !fragmentedSegmentsQueue_.empty();
}

bool Cutter::areLimitsBroken_(size_t segmentLength) {
    return isSoftLimitBroken_(segmentLength)
        || isHardLimitBroken_(segmentLength);
}

bool Cutter::isSoftLimitBroken_(size_t segmentLength) {
    return isSoftLimitSet_() && segmentLength >= segmentLengthSoftLimit();
}

bool Cutter::isSoftLimitSet_() {
    return segmentLengthSoftLimit() != 0;
}

bool Cutter::isHardLimitBroken_(size_t segmentLength) {
    return isHardLimitSet_() && segmentLength >= segmentLengthHardLimit();
}

bool Cutter::isHardLimitSet_() {
    return segmentLengthHardLimit() != 0;
}

bool Cutter::isSoftLimitCharacter_(char c) const {
    return c == ' ';
}
