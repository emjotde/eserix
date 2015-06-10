#pragma once

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include "regexp.hpp"
#include "srx_rules.hpp"
#include "utf8.h"

#include "cutter.hpp"

class SrxSentenceCutter;

class SrxSegmenter {

public:
    SrxSegmenter(const std::string& lang,
                 boost::filesystem::path rules,
                 size_t hardLimit,
                 size_t softLimit,
                 bool cascade=false);

private:

    friend class RuleProcessor;
    friend class SrxSentenceCutter;

    void processRule_(const SrxRule& srxRule);
    std::string makeRegexp_(const SrxRule& srxRule);
    std::string makeAllParensNonCapturing_(const std::string& pattern);

    std::vector<boost::shared_ptr<PerlRegExp> > nonBreakingRules_;

    struct BreakingRuleInfo {
        boost::shared_ptr<PerlRegExp> breakingRule;
        size_t nbOfApplicableNonBreakingRules;


        BreakingRuleInfo(boost::shared_ptr<PerlRegExp> aBreakingRule,
                         size_t aNbOfApplicableNonBreakingRules)
            : breakingRule(aBreakingRule),
              nbOfApplicableNonBreakingRules(aNbOfApplicableNonBreakingRules) {
        }
    };

    std::string langCode_;

    std::vector<BreakingRuleInfo> breakingRules_;

    size_t hardLimit_;
    size_t softLimit_;
};

class RuleProcessor {
public:
    RuleProcessor(SrxSegmenter& segmenter)
        :segmenter_(segmenter) {
    }

    void operator()(const SrxRule& srxRule) {
        segmenter_.processRule_(srxRule);
    }

    private:
    SrxSegmenter& segmenter_;

};

void trim(std::string&);

class SrxSentenceCutter : private Cutter {
public:
    SrxSentenceCutter(SrxSegmenter& segmenter, const std::string& line, bool trim)
        :segmenter_(segmenter), line_(line), trim_(trim), pos_(0), good_(true),
         breakingRuleApplications_(segmenter.breakingRules_.size()),
         nonBreakingRuleApplications_(segmenter.nonBreakingRules_.size()) {
    }

    SrxSentenceCutter& operator>>(std::string& frag) {
        if(pos_ == std::string::npos) {
            good_ = false;
            return *this;
        }
        AnnotationItem a = cutOff(line_, pos_);
        frag = a.getText();
        if(trim_)
            trim(frag);
        return *this;
    }
    
    operator bool() {
        return good_;
    }
    
private:
    const static std::string DEFAULT_SENTENCE_CATEGORY;

    virtual AnnotationItem doCutOff(const std::string& text, size_t& positionInText) {
        return performCutOff(text, positionInText);
    }

    virtual AnnotationItem doCutOff(const StringFrag& text, size_t& positionInText) {
        return performCutOff(text, positionInText);
    }

    template <typename StringType>
    AnnotationItem performCutOff(const StringType& text, size_t& positionInText) {
        size_t nearestBreakPoint = 0;
        size_t nearestRuleIndex = 0;
        size_t minBreakPoint =
            positionInText + utf8::unchecked::sequence_length(text.data() + positionInText);
        bool candidateFound = false;
        bool candidateAccepted = false;

        assert (segmenter_.breakingRules_.size() == breakingRuleApplications_.size());
        assert (segmenter_.nonBreakingRules_.size() == nonBreakingRuleApplications_.size());

        do {
            candidateFound = false;

            for (size_t i = 0; i < segmenter_.breakingRules_.size(); ++i) {
                size_t ruleBreakPoint = updateBreakingRuleIndex_(i,
                                                                 minBreakPoint,
                                                                 text,
                                                                 positionInText);

                assert (ruleBreakPoint == std::string::npos
                        || ruleBreakPoint >= minBreakPoint);

                if (ruleBreakPoint != std::string::npos
                    && (!candidateFound || ruleBreakPoint < nearestBreakPoint)) {
                    nearestBreakPoint = ruleBreakPoint;
                    nearestRuleIndex = i;
                    candidateFound = true;
                }
            }

            if (candidateFound) {
                candidateAccepted = checkBreakPoint_(
                    nearestBreakPoint,
                    text,
                    positionInText,
                    segmenter_.breakingRules_[nearestRuleIndex].nbOfApplicableNonBreakingRules);

                if (!candidateAccepted)
                    minBreakPoint =
                        nearestBreakPoint
                        + utf8::unchecked::sequence_length(text.data() + nearestBreakPoint);
            }

            assert (!(!candidateFound && candidateAccepted));
        } while (candidateFound && !candidateAccepted);

        if (candidateAccepted) {
            assert (candidateFound);

            size_t currentPosition = positionInText;

            assert (nearestBreakPoint > currentPosition);
            size_t sentenceLength = nearestBreakPoint - currentPosition;

            positionInText += sentenceLength;

            return AnnotationItem(
                DEFAULT_SENTENCE_CATEGORY,
                StringFrag(text, currentPosition, sentenceLength));
        }
        else {
            size_t currentPosition = positionInText;
            positionInText = std::string::npos;
            return AnnotationItem(
                DEFAULT_SENTENCE_CATEGORY,
                StringFrag(text, currentPosition, text.length() - currentPosition));
        }
    }

    virtual void doReset() {
        BOOST_FOREACH(RuleApplication& ruleApplication, breakingRuleApplications_) {
            ruleApplication = RuleApplication();
        }

        BOOST_FOREACH(RuleApplication& ruleApplication, nonBreakingRuleApplications_) {
            ruleApplication = RuleApplication();
        }
    }

    virtual size_t doSegmentLengthHardLimit() {
        return segmenter_.hardLimit_;
    }

    virtual size_t doSegmentLengthSoftLimit() {
        return segmenter_.softLimit_;
    }


    template <typename StringType>
    size_t updateBreakingRuleIndex_(size_t breakingRuleIndex,
                                    size_t minBreakPoint,
                                    const StringType& text,
                                    size_t positionInText) {
        SrxSegmenter::BreakingRuleInfo& ruleInfo = segmenter_.breakingRules_[breakingRuleIndex];

        return updatePosition_(ruleInfo.breakingRule,
                               breakingRuleApplications_[breakingRuleIndex],
                               minBreakPoint,
                               text,
                               positionInText);
    }

    template <typename StringType>
    bool checkBreakPoint_(size_t breakPoint,
                          const StringType& text,
                          size_t positionInText,
                          size_t nbOfRulesToCheck) {

        assert (nbOfRulesToCheck <= nonBreakingRuleApplications_.size());

        for (size_t i = 0; i < nbOfRulesToCheck; ++i) {
            size_t ruleNonBreakPoint = updateNonBreakingRuleIndex_(i,
                                                                   breakPoint,
                                                                   text,
                                                                   positionInText);

            if (ruleNonBreakPoint != std::string::npos
                && ruleNonBreakPoint == breakPoint)
                return false;
        }

        return true;
    }

    template <typename StringType>
    size_t updateNonBreakingRuleIndex_(size_t breakingRuleIndex,
                                       size_t minBreakPoint,
                                       const StringType& text,
                                       size_t positionInText) {
        return updatePosition_(segmenter_.nonBreakingRules_[breakingRuleIndex],
                               nonBreakingRuleApplications_[breakingRuleIndex],
                               minBreakPoint,
                               text,
                               positionInText);
    }

    SrxSegmenter& segmenter_;
    std::string line_;
    bool trim_;
    size_t pos_;
    bool good_;

    struct RuleApplication {
        size_t startingPosition;
        size_t breakingPosition;

        RuleApplication()
            :startingPosition(0U), breakingPosition(std::string::npos) {
        }
    };

    std::vector<RuleApplication> breakingRuleApplications_;
    std::vector<RuleApplication> nonBreakingRuleApplications_;

    template <typename StringType>
    size_t updatePosition_(boost::shared_ptr<PerlRegExp> regexp,
                           RuleApplication& ruleApplication,
                           size_t minBreakPoint,
                           const StringType& text,
                           size_t positionInText) {

        if (ruleApplication.startingPosition < positionInText)
            ruleApplication.startingPosition = positionInText;

        while (ruleApplication.startingPosition < text.length()
               && (ruleApplication.breakingPosition == std::string::npos
                   || ruleApplication.breakingPosition < minBreakPoint)) {
            PerlStringPiece currentText(text.data() + ruleApplication.startingPosition, text.length() - ruleApplication.startingPosition);
            int originalLength = currentText.size();
            PerlStringPiece fragFound;

            if (PerlRegExp::FindAndConsume(&currentText, *regexp.get(), &fragFound)) {
                assert (originalLength > currentText.size());
                int lengthDiff = originalLength - currentText.size();

                ruleApplication.breakingPosition =
                    ruleApplication.startingPosition + (lengthDiff - fragFound.size());

                assert (ruleApplication.breakingPosition >= ruleApplication.startingPosition);

                ruleApplication.startingPosition +=
                    utf8::unchecked::sequence_length(
                        text.data() + ruleApplication.startingPosition);
            }
            else {
                ruleApplication.breakingPosition = std::string::npos;
                ruleApplication.startingPosition = text.length();
            }
        }

        if (ruleApplication.breakingPosition < minBreakPoint)
            ruleApplication.breakingPosition = std::string::npos;

        return ruleApplication.breakingPosition;
    }
};
