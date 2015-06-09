#pragma once

#include <string>

class StringFrag {
private:

    static const int ABBR_LENGTH = 4;

public:

    StringFrag() :
        contents_(""),
        src_(contents_),
        begin_(std::string::npos),
        len_(std::string::npos)
    { }

    StringFrag(const StringFrag & other) :
        contents_(other.contents_),
        src_(other.src_),
        begin_(other.begin_),
        len_(other.len_)
    { }

    StringFrag(
        const std::string & src,
        size_t begin,
        size_t len
    ) :
#if STRING_FRAG_VALIDATION
        contents_(src.substr(begin, (std::min)(len, (size_t)ABBR_LENGTH))),
#else
        contents_(""),
#endif
        src_(src),
        begin_(begin),
        len_(len)
    {
#if STRING_FRAG_VALIDATION
        if (len == std::string::npos) {
            //throw StringFragException("String frag's length initialized as npos");
        }
#endif
    }

    StringFrag(
        const StringFrag & sf,
        size_t begin,
        size_t len
    ) :
#if STRING_FRAG_VALIDATION
        contents_(sf.src_.substr(sf.begin_ + begin, (std::min)(len, (size_t)ABBR_LENGTH))),
#else
        contents_(""),
#endif
        src_(sf.src_),
        begin_(sf.begin_ + begin),
        len_(len)
    {
#if STRING_FRAG_VALIDATION
        if (len == std::string::npos) {
            //throw StringFragException("String frag's length initialized as npos");
        }
#endif
    }

    StringFrag(
        std::string contents
    ) :
        contents_(contents),
        src_(contents_),
        begin_(std::string::npos),
        len_(std::string::npos)
    { }

    StringFrag & operator=(const StringFrag & other);

    const char & operator[](size_t pos) const;

    std::string str() const;

    std::string substr(size_t pos = 0, size_t n = std::string::npos) const;

    void append(const StringFrag & other);

    size_t find(char c, size_t pos = 0) const;

    size_t length() const;

    const char* data() const;

#if STRING_FRAG_VALIDATION
    bool valid() const;
#endif

private:

    std::string contents_;
    const std::string & src_;
    size_t begin_;
    size_t len_;

    bool stored_() const {
        return begin_ == std::string::npos;
    }
};

