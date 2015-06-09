#include "string_frag.hpp"


StringFrag & StringFrag::operator=(const StringFrag & other) {

#if STRING_FRAG_VALIDATION
    if (!valid()) {
        throw StringFragException("String frag invalidated (" + contents_ + "...)");
    }
    if (!other.valid()) {
        throw StringFragException("String frag invalidated (" + other.contents_ + "...)");
    }
#endif

    if (this != &other) {
        this->StringFrag::~StringFrag();
        new (this) StringFrag(other);
    }
    return *this;
}


const char & StringFrag::operator[](size_t pos) const {

#if STRING_FRAG_VALIDATION
    if (!valid()) {
        throw StringFragException("String frag invalidated (" + contents_ + "...)");
    }
#endif

    if (stored_()) {
        return contents_[pos];
    }
    return src_[begin_ + pos];
}


std::string StringFrag::str() const {

#if STRING_FRAG_VALIDATION
    if (!valid()) {
        throw StringFragException("String frag invalidated (" + contents_ + "...)");
    }
#endif

    if (stored_()) {
        return contents_;
    }
    return src_.substr(begin_, len_);
}


std::string StringFrag::substr(size_t pos, size_t n) const {
    return str().substr(pos, n);
}


void StringFrag::append(const StringFrag & other) {

#if STRING_FRAG_VALIDATION
    if (!valid()) {
        throw StringFragException("String frag invalidated (" + contents_ + "...)");
    }
    if (!other.valid()) {
        throw StringFragException("String frag invalidated (" + other.contents_ + "...)");
    }
#endif

    if (
        !stored_() &&
        !other.stored_() &&
        src_ == other.src_ &&
        begin_ + len_ == other.begin_
    ) {
        len_ += other.len_;
        contents_ = src_.substr(begin_, (std::min)(len_, (size_t)ABBR_LENGTH));
    } else if (contents_=="" && this != &other) {
        this->StringFrag::~StringFrag();
        new (this) StringFrag(other);
    } else {
        contents_ = str() + other.str();
        begin_ = std::string::npos;
    }
}


size_t StringFrag::find(char c, size_t pos) const {

#if STRING_FRAG_VALIDATION
    if (!valid()) {
        throw StringFragException("String frag invalidated (" + contents_ + "...)");
    }
#endif

    if (stored_()) {
        return contents_.find(c, pos);
    }
    return src_.find(c, begin_ + pos) - begin_;
}


size_t StringFrag::length() const {
    if (stored_()) {
        return contents_.length();
    }
    return len_;
}


const char* StringFrag::data() const {
    if (stored_()) {
        return contents_.data();
    }
    return src_.data() + begin_;
}


#if STRING_FRAG_VALIDATION
bool StringFrag::valid() const {
    if (
        stored_()
        || src_.substr(begin_, (len_ < ABBR_LENGTH ? len_ : ABBR_LENGTH)) == contents_
    ) return true;
    return false;
}
#endif
