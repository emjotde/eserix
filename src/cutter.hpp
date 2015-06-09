#ifndef CUTTER_HDR
#define CUTTER_HDR

#include <list>
#include <string>
#include <queue>
#include <iostream>

#include "annotation_item.hpp"
#include "string_frag.hpp"
#include "psi_exception.hpp"
#include "string_helpers.hpp"

class Cutter {

public:

    template <typename StringType>
    AnnotationItem cutOff(const StringType& text, size_t& positionInText);

    void reset();

    size_t segmentLengthHardLimit();

    size_t segmentLengthSoftLimit();

    virtual ~Cutter();

    class Exception : public PsiException {
    public:
        Exception(const std::string& msg): PsiException(msg) {
        }

        virtual ~Exception() throw() {}
    };

private:
    virtual AnnotationItem doCutOff(const std::string& text, size_t& positionInText) = 0;
    virtual AnnotationItem doCutOff(const StringFrag& text, size_t& positionInText) = 0;

    virtual void doReset() = 0;
    virtual size_t doSegmentLengthHardLimit() = 0;
    virtual size_t doSegmentLengthSoftLimit() = 0;

    bool shouldFragmentQueueBeUsed_() const;

    template <typename StringType>
    AnnotationItem getFirstItemInFragmentQueue_(const StringType& text, size_t& positionInText);

    bool areLimitsBroken_(size_t segmentLength);
    bool isSoftLimitBroken_(size_t segmentLength);
    bool isSoftLimitSet_();
    bool isHardLimitBroken_(size_t segmentLength);
    bool isHardLimitSet_();

    template <typename StringType>
    void fragmentSegment_(
        const AnnotationItem& item,
        const StringType& text,
        size_t positionInText,
        size_t segmentLength);

    template <typename StringType>
    size_t findSoftLimitCutPoint_(
        const StringType& text,
        size_t cutPoint,
        size_t maxCutPoint);

    template <typename StringType>
    size_t findHardLimitCutPoint_(
        const StringType& text,
        size_t cutPoint);

    bool isSoftLimitCharacter_(char c) const;

    std::queue<AnnotationItem> fragmentedSegmentsQueue_;
};


template <typename StringType>
AnnotationItem Cutter::cutOff(const StringType& text, size_t& positionInText) {
    if (shouldFragmentQueueBeUsed_())
        return getFirstItemInFragmentQueue_(text, positionInText);

    size_t prevPosition = positionInText;
    AnnotationItem item = doCutOff(text, positionInText);

    size_t realPositionInText =
        (positionInText == std::string::npos
         ? text.length()
         : positionInText);

    size_t segmentLength = realPositionInText - prevPosition;

    if (areLimitsBroken_(segmentLength)) {
        positionInText = prevPosition;
        fragmentSegment_(item, text, positionInText, segmentLength);

        return getFirstItemInFragmentQueue_(text, positionInText);
    }

    return item;
}


template <typename StringType>
AnnotationItem Cutter::getFirstItemInFragmentQueue_(
    const StringType& text, size_t& positionInText) {

    AnnotationItem item = fragmentedSegmentsQueue_.front();
    fragmentedSegmentsQueue_.pop();

    positionInText += item.getText().length();

    if (positionInText == text.length())
        positionInText = std::string::npos;

    return item;
}


template <typename StringType>
void Cutter::fragmentSegment_(
    const AnnotationItem& item,
    const StringType& text,
    size_t positionInText,
    size_t segmentLength) {

    size_t cutPoint = positionInText;

    while (cutPoint < positionInText + segmentLength) {
        size_t currentLength = segmentLength - (cutPoint - positionInText);

        size_t softLimitCutPoint =
            isSoftLimitBroken_(currentLength)
            ? findSoftLimitCutPoint_(text, cutPoint, positionInText + segmentLength)
            : std::string::npos;

        size_t hardLimitCutPoint =
            isHardLimitBroken_(currentLength)
            ? findHardLimitCutPoint_(text, cutPoint)
            : std::string::npos;

        size_t prevCutPoint = cutPoint;

        if (softLimitCutPoint != std::string::npos && hardLimitCutPoint != std::string::npos)
            cutPoint = (std::min)(softLimitCutPoint, hardLimitCutPoint);
        else if (softLimitCutPoint != std::string::npos)
            cutPoint = softLimitCutPoint;
        else if (hardLimitCutPoint != std::string::npos)
            cutPoint = hardLimitCutPoint;
        else
            cutPoint = positionInText + segmentLength;

        AnnotationItem fragmentItem(item, StringFrag(text, prevCutPoint, cutPoint - prevCutPoint));

        fragmentedSegmentsQueue_.push(fragmentItem);
    }
}


template <typename StringType>
size_t Cutter::findSoftLimitCutPoint_(
    const StringType& text,
    size_t cutPoint,
    size_t maxCutPoint) {

    cutPoint += segmentLengthSoftLimit();

    while (cutPoint < maxCutPoint) {
        if (isSoftLimitCharacter_(text[cutPoint]))
            return cutPoint;

        ++cutPoint;
    }

    return maxCutPoint;
}


template <typename StringType>
size_t Cutter::findHardLimitCutPoint_(
    const StringType& text,
    size_t cutPoint) {

    const size_t UTF8_MAX_LENGTH = 4;

    size_t hardLimitPos = cutPoint + segmentLengthHardLimit();

    size_t candidatePos =
        (hardLimitPos > UTF8_MAX_LENGTH
         ? hardLimitPos - UTF8_MAX_LENGTH
         : 0);

    while (candidatePos < hardLimitPos) {
        int symLen = symbolLength(text, candidatePos);
        if (candidatePos + symLen > hardLimitPos)
            break;
        candidatePos += symLen;
    }

    if (candidatePos <= cutPoint)
        throw Exception(
            "segment hard limit is so small "
            "that a Unicode character cannot be contained in it, make hard limit larger");

    return candidatePos;
}


#endif
