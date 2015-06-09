#ifndef STRING_HELPERS_HDR_HDR
#define STRING_HELPERS_HDR_HDR

#include <string>

#include "string_frag.hpp"

size_t symbolLength(const std::string& s, int ix);

size_t symbolLength(const StringFrag& s, int ix);

size_t symbolLength(const char* s, int ix);

size_t symbolLength(const char* s);

std::string getLemmaFromLexeme(const std::string& lexeme);

#endif
