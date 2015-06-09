#include "string_helpers.hpp"

#include "utf8.h"

size_t symbolLength(const std::string& s, int ix) {
    return symbolLength(s.c_str(), ix);
}

size_t symbolLength(const StringFrag& s, int ix) {
    return symbolLength(s.str(), ix);
}

size_t symbolLength(const char* s, int ix) {
    return symbolLength(s + ix);
}

size_t symbolLength(const char* s) {
    size_t len = utf8::unchecked::sequence_length(s);

    return len > 0 ? len : 1;
}

