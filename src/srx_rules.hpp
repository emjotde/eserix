#pragma once

#include <string>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#include "xml_property_tree.hpp"
#include "psi_exception.hpp"

class SrxRule {

public:
    SrxRule(bool breakable, const std::string& beforeBreak, const std::string& afterBreak);

    bool isBreakable() const;
    const std::string& getBeforeBreak() const;
    const std::string& getAfterBreak() const;

private:
    bool breakable_;
    std::string beforeBreak_;
    std::string afterBreak_;
};

class SrxRulesReader {
public:
    class Exception : public PsiException {
    public:
        Exception(const std::string& msg)
            :PsiException(msg) {
        }

        virtual ~Exception() throw() {
        }
    };

    class UnexpectedElementException : public Exception {
    public:
        UnexpectedElementException(const std::string& elementName)
            :Exception(
                std::string("unexpected element `")
                + elementName
                + "'") {
        }
    };

    SrxRulesReader(
        const boost::filesystem::path& filePath,
        const std::string& lang,
        bool forceCascade=false);

    template<typename Out>
    void getRules(Out outputIterator) {
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                      xmlParsed_->get_child("srx.body")) {
            if (v.first == "languagerules")
                processLanguageRules_(v.second, outputIterator);
        }
    }

private:

    template<typename Out>
    void processLanguageRules_(boost::property_tree::ptree& languageRules, Out outputIterator) {
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                      languageRules.get_child("")) {
            if (v.first == "languagerule")
                processLanguageRule_(v.second, outputIterator);
            else
                throw UnexpectedElementException(v.first);
        }
    }


    template<typename Out>
    void processLanguageRule_(boost::property_tree::ptree& languageRule, Out outputIterator) {
        std::string languageRuleName = languageRule.get<std::string>("<xmlattr>.languagerulename");

        if (languageRuleNames_.count(languageRuleName)) {
            BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                          languageRule.get_child("")) {
                if (v.first == "rule")
                    processRule_(v.second, outputIterator);
                else
                    UnexpectedElementException(v.first);
            }
        }
    }

    template<typename Out>
    void processRule_(boost::property_tree::ptree& rule, Out outputIterator) {
        std::string breakableAttr = rule.get<std::string>("<xmlattr>.break");

        bool breakable =
            (breakableAttr == "yes"
             ? true
             : (breakableAttr == "no"
                ? false : throw Exception("expected `yes' or `no'")));

        std::string beforeBreak = rule.get<std::string>("beforebreak");
        std::string afterBreak = rule.get<std::string>("afterbreak");

        SrxRule srxRule(breakable, beforeBreak, afterBreak);

        *outputIterator++ = srxRule;
    }

    void findLanguageRuleNames_();
    bool processMapRules_(boost::property_tree::ptree& maprules);
    bool processMapRule_(boost::property_tree::ptree& maprules);
    bool processLanguageMap_(boost::property_tree::ptree& langmap);

    void setCascade_();
    bool cascadeAttrToBool_(boost::optional<std::string> attrValue);
    bool yesNoToBool_(const std::string& yesNo);

    std::set<std::string> languageRuleNames_;

    boost::shared_ptr<XmlPropertyTree> xmlParsed_;
    const std::string& lang_;
    bool forceCascade_;
    unsigned int srxVersion_;
    bool cascade_;
};


