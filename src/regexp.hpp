#pragma once

#include <pcrecpp.h>
#include "regexp_variadic_function.hpp"

class PCREWrapper;

typedef PCREWrapper RegExp;
typedef PCREWrapper PerlRegExp;
typedef pcrecpp::StringPiece StringPiece;
typedef pcrecpp::Arg Arg;
typedef pcrecpp::StringPiece PerlStringPiece;
typedef pcrecpp::Arg PerlArg;

class PCREWrapper : private pcrecpp::RE {
    public:
        
        pcrecpp::RE_Options UTF8() {
            pcrecpp::RE_Options options = pcrecpp::UTF8();
            // enable unicode \b and \w
            options.set_all_options(options.all_options()|PCRE_UCP);
            return options;
        }
        
        PCREWrapper(const std::string &pattern) :
            pcrecpp::RE(pattern, UTF8()) {}
        PCREWrapper(const std::string &pattern, const pcrecpp::RE_Options &option) :
            pcrecpp::RE(pattern, option) {}

        PCREWrapper(const char *pattern) :
            pcrecpp::RE(pattern, UTF8()) {}
        PCREWrapper(const char *pattern, const pcrecpp::RE_Options &option) :
            pcrecpp::RE(pattern, option) {}

        static bool FullMatchN(const PerlStringPiece& text, const PCREWrapper& re,
                const PerlArg* const args[], int argc);
        static const VariadicFunction2<
            bool, const PerlStringPiece&, const PCREWrapper&, PerlArg,
            PCREWrapper::FullMatchN> FullMatch;

        static bool PartialMatchN(const PerlStringPiece& text, const PCREWrapper& re,
                const PerlArg* const args[], int argc);
        static const VariadicFunction2<
            bool, const PerlStringPiece&, const PCREWrapper&, PerlArg,
            PCREWrapper::PartialMatchN> PartialMatch;

        static bool ConsumeN(PerlStringPiece* input, const PCREWrapper& re,
                const PerlArg* const args[], int argc);
        static const VariadicFunction2<
            bool, PerlStringPiece*, const PCREWrapper&, PerlArg,
            PCREWrapper::ConsumeN> Consume;

        static bool FindAndConsumeN(PerlStringPiece* input, const PCREWrapper& re,
                const PerlArg* const args[], int argc);
        static const VariadicFunction2<
            bool, PerlStringPiece*, const PCREWrapper&, PerlArg,
            PCREWrapper::FindAndConsumeN> FindAndConsume;

        static bool Replace(std::string *str,
                const PCREWrapper& pattern,
                const PerlStringPiece& rewrite) {
            return ((pcrecpp::RE)pattern).Replace(rewrite, str);
        }

        static int GlobalReplace(std::string *str,
                const PCREWrapper& pattern,
                const PerlStringPiece& rewrite) {
            return ((pcrecpp::RE)pattern).GlobalReplace(rewrite, str);
        }

        static bool Extract(const PerlStringPiece &text,
                const PCREWrapper& pattern,
                const PerlStringPiece &rewrite,
                std::string *out) {
            return ((pcrecpp::RE)pattern).Extract(rewrite, text, out);
        }

        int NumberOfCapturingGroups() const {
            return ((pcrecpp::RE)(*this)).NumberOfCapturingGroups();
        }

        const string& error() const {
            return ((pcrecpp::RE)(*this)).error();
        }

        bool ok() const {
            return ((pcrecpp::RE)(*this)).error().empty();
        }

        const string& pattern() const {
            return ((const pcrecpp::RE&)(*this)).pattern();
        }

        static const int MAX_MATCHES;

        //@todo: any other methods of pcrecpp::RE ?
};
