#include "annotation_item.hpp"

bool AnnotationItem::operator==(const AnnotationItem& other) const {
    return getCategory() == other.getCategory()
        && getText() == other.getText()
        && areAttributesTheSame_(other);
}

bool AnnotationItem::operator!=(const AnnotationItem& other) const {
    return !operator==(other);
}

std::string AnnotationItem::getCategory() const {
    return category_;
}

std::string AnnotationItem::getText() const {
    return text_.str();
}

StringFrag AnnotationItem::getTextAsStringFrag() const {
    return text_;
}

long AnnotationItem::getHash() const {
    std::string str = category_;
    str += text_.str();
    BOOST_FOREACH(const zvalue & av, values_) {
        char * avStr = zvalue_to_string(av);
        str += avStr;
        delete [] avStr;
    }
    const std::collate<char>& coll = std::use_facet<std::collate<char> >(std::locale());
    return coll.hash(str.data(), str.data() + str.length());
}

size_t AnnotationItem::resize_(size_t size) {
    if (size > values_.size()) {
        values_.resize(size);
    }
    assert (size <= attributes_.size()); // TODO resize if size > 64
    return attributes_.size();
}

bool AnnotationItem::areAttributesTheSame_(const AnnotationItem& other) const {
    size_t smallerSize = values_.size();
    size_t largerSize  = other.values_.size();
    const std::vector<zvalue>* largerVector = &(other.values_);

    if (smallerSize > largerSize) {
        smallerSize = other.values_.size();
        largerSize  = values_.size();
        largerVector = &values_;
    }

   for (size_t i = 0; i < smallerSize; ++i) {
       if (values_[i] != other.values_[i]) {
           return false;
       }
   }

   for (size_t i = smallerSize; i < largerSize; ++i) {
       char * valStr = zvalue_to_string((*largerVector)[i]);
       if (strcmp(valStr, "") != 0) {
           delete [] valStr;
           return false;
       }
   }

   return true;
}
