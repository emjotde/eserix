#include "regexp.hpp"

const VariadicFunction2<bool, const PerlStringPiece&,
                        const PCREWrapper&, PerlArg,
                        PCREWrapper::FullMatchN> PCREWrapper::FullMatch;

const VariadicFunction2<bool, const PerlStringPiece&,
                        const PCREWrapper&, PerlArg,
                        PCREWrapper::PartialMatchN> PCREWrapper::PartialMatch;

const VariadicFunction2<bool, PerlStringPiece*,
                        const PCREWrapper&, PerlArg,
                        PCREWrapper::ConsumeN> PCREWrapper::Consume;

const VariadicFunction2<bool, PerlStringPiece*,
                        const PCREWrapper&, PerlArg,
                        PCREWrapper::FindAndConsumeN> PCREWrapper::FindAndConsume;

bool PCREWrapper::FullMatchN(const PerlStringPiece& text,
        const PCREWrapper& re, const PerlArg* const args[], int argc) {
    switch (argc) {
        case 0: return ((pcrecpp::RE)re).FullMatch(text); break;
        case 1: return ((pcrecpp::RE)re).FullMatch(text, *args[0]); break;
        case 2: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1]); break;
        case 3: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2]); break;
        case 4: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3]);
                break;
        case 5: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4]); break;
        case 6: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5]); break;
        case 7: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6]); break;
        case 8: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6], *args[7]); break;
        case 9: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6], *args[7], *args[8]); break;
        case 10: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9]); break;
        case 11: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10]); break;
        case 12: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11]); break;
        case 13: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12]); break;
        case 14: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13]); break;
        case 15: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13],
                         *args[14]); break;
        case 16: return ((pcrecpp::RE)re).FullMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13],
                         *args[14], *args[15]); break;
        default: return ((pcrecpp::RE)re).FullMatch(text);
    }
}

bool PCREWrapper::PartialMatchN(const PerlStringPiece& text,
        const PCREWrapper& re, const PerlArg* const args[], int argc) {
    switch (argc) {
        case 0: return ((pcrecpp::RE)re).PartialMatch(text); break;
        case 1: return ((pcrecpp::RE)re).PartialMatch(text, *args[0]); break;
        case 2: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1]); break;
        case 3: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2]); break;
        case 4: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3]);
                break;
        case 5: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4]); break;
        case 6: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5]); break;
        case 7: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6]); break;
        case 8: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6], *args[7]); break;
        case 9: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6], *args[7], *args[8]); break;
        case 10: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9]); break;
        case 11: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10]); break;
        case 12: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11]); break;
        case 13: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12]); break;
        case 14: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13]); break;
        case 15: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13],
                         *args[14]); break;
        case 16: return ((pcrecpp::RE)re).PartialMatch(text, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13],
                         *args[14], *args[15]); break;
        default: return ((pcrecpp::RE)re).PartialMatch(text);
    }
}

bool PCREWrapper::ConsumeN(PerlStringPiece* input,
        const PCREWrapper& re, const PerlArg* const args[], int argc) {
    switch (argc) {
        case 0: return ((pcrecpp::RE)re).Consume(input); break;
        case 1: return ((pcrecpp::RE)re).Consume(input, *args[0]); break;
        case 2: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1]); break;
        case 3: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2]); break;
        case 4: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3]);
                break;
        case 5: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                        *args[4]); break;
        case 6: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5]); break;
        case 7: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6]); break;
        case 8: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6], *args[7]); break;
        case 9: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                        *args[4], *args[5], *args[6], *args[7], *args[8]); break;
        case 10: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9]); break;
        case 11: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10]); break;
        case 12: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11]); break;
        case 13: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12]); break;
        case 14: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13]); break;
        case 15: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13],
                         *args[14]); break;
        case 16: return ((pcrecpp::RE)re).Consume(input, *args[0], *args[1], *args[2], *args[3],
                         *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13],
                         *args[14], *args[15]); break;
        default: return ((pcrecpp::RE)re).Consume(input);
    }
}

bool PCREWrapper::FindAndConsumeN(PerlStringPiece* input,
        const PCREWrapper& re, const PerlArg* const args[], int argc) {
    switch (argc) {
        case 0: return ((pcrecpp::RE)re).FindAndConsume(input); break;
        case 1: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0]); break;
        case 2: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1]); break;
        case 3: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2]);
                break;
        case 4: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                        *args[3]); break;
        case 5: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                        *args[3], *args[4]); break;
        case 6: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                        *args[3], *args[4], *args[5]); break;
        case 7: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                        *args[3], *args[4], *args[5], *args[6]); break;
        case 8: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                        *args[3], *args[4], *args[5], *args[6], *args[7]); break;
        case 9: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                        *args[3], *args[4], *args[5], *args[6], *args[7], *args[8]); break;
        case 10: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                         *args[3], *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9]); break;
        case 11: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                         *args[3], *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10]); break;
        case 12: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                         *args[3], *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11]); break;
        case 13: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                         *args[3], *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12]); break;
        case 14: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                         *args[3], *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13]); break;
        case 15: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                         *args[3], *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13],
                         *args[14]); break;
        case 16: return ((pcrecpp::RE)re).FindAndConsume(input, *args[0], *args[1], *args[2],
                         *args[3], *args[4], *args[5], *args[6], *args[7], *args[8],
                         *args[9], *args[10], *args[11], *args[12], *args[13],
                         *args[14], *args[15]); break;
        default: return ((pcrecpp::RE)re).FindAndConsume(input);
    }
}

const int PCREWrapper::MAX_MATCHES = 16;
