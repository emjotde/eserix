#include "srx_segmenter.hpp"

#include <cassert>
#include <cctype>
#include <boost/function_output_iterator.hpp>

#include "srx_rules.hpp"

#include "escaping.hpp"

void trim(std::string& toTrim) {
    size_t pos1 = 0;
    size_t pos2 = toTrim.length();
    
    while(std::isspace(toTrim[pos1]))
        pos1++;
    while(std::isspace(toTrim[pos2 - 1]))
        pos2--;
    
    toTrim = toTrim.substr(pos1, pos2 - pos1);
}

void SrxSegmenter::processRule_(const SrxRule& srxRule) {
    boost::shared_ptr<PerlRegExp> ruleRegexp(
        new PerlRegExp(makeRegexp_(srxRule)));

    if (srxRule.isBreakable()) {
        size_t nbOfNonBreakingRules = nonBreakingRules_.size();

        breakingRules_.push_back(
            BreakingRuleInfo(ruleRegexp, nbOfNonBreakingRules));
    }
    else
        nonBreakingRules_.push_back(ruleRegexp);
}

std::string SrxSegmenter::makeRegexp_(const SrxRule& srxRule) {
    return
        std::string("(?:")
        + makeAllParensNonCapturing_(srxRule.getBeforeBreak())
        + std::string(")(")
        + makeAllParensNonCapturing_(srxRule.getAfterBreak())
        + std::string(")");
}

std::string SrxSegmenter::makeAllParensNonCapturing_(const std::string& pattern) {
    size_t pos = 0;
    std::string ret = pattern;
    while ( (pos = ret.find("(", pos)) != std::string::npos ) {
        if (!Escaping::isEscaped(ret, pos)
            && !(pos + 1 < ret.length() && ret[pos+1] == '?')) {
            ret.replace(pos+1, 0, "?:");
            pos += 2;
        }

        ++pos;
    }

    return ret;
}

SrxSegmenter::SrxSegmenter(
    const std::string& lang,
    boost::filesystem::path rules,
    size_t hardLimit,
    size_t softLimit,
    bool cascade):
    langCode_(lang), hardLimit_(hardLimit), softLimit_(softLimit) {

    SrxRulesReader ruleReader(rules, lang, cascade);
    RuleProcessor ruleProc(*this);

    ruleReader.getRules(boost::make_function_output_iterator(ruleProc));
}

const std::string SrxSentenceCutter::DEFAULT_SENTENCE_CATEGORY = "sen";

