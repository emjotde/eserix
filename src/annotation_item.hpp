#pragma once

#include <bitset>
#include <locale>
#include <string>
#include <vector>

#include <boost/dynamic_bitset.hpp>
#include <boost/foreach.hpp>

#include "string_frag.hpp"
#include "zvalue.hpp"

class AnnotationItem {

public:
    AnnotationItem(const std::string& category) :
        category_(category),
        attributes_()
    { }

    AnnotationItem(const std::string& category, StringFrag text) :
        category_(category),
        text_(text),
        attributes_()
    { }

    AnnotationItem(const std::string& category, int /* size */) :
        category_(category),
        attributes_()
    { }

    AnnotationItem(const std::string& category, StringFrag text, int /* size */) :
        category_(category),
        text_(text),
        attributes_()
    { }

    AnnotationItem(const AnnotationItem& item, const std::string& newCategory) :
        category_(newCategory),
        text_(item.text_),
        values_(item.values_),
        attributes_(item.attributes_) {
    }

    AnnotationItem(
        const AnnotationItem& item,
        const std::string& newCategory,
        StringFrag newText) :
        category_(newCategory),
        text_(newText),
        values_(item.values_),
        attributes_(item.attributes_) {
    }

    AnnotationItem(const AnnotationItem& item, StringFrag newText) :
        category_(item.category_),
        text_(newText),
        values_(item.values_),
        attributes_(item.attributes_) {
    }

    AnnotationItem(const AnnotationItem& item) :
        category_(item.category_),
        text_(item.text_),
        values_(item.values_),
        attributes_(item.attributes_) {
    }

    std::string getCategory() const;

    std::string getText() const;

    StringFrag getTextAsStringFrag() const;

    long getHash() const;

    bool operator==(const AnnotationItem& other) const;
    bool operator!=(const AnnotationItem& other) const;

    friend class AnnotationItemManager;

private:

    /**
     * Stores the annotation item's category.
     */
    std::string category_;

    /**
     * Stores the annotation item's text as a string frag.
     */
    StringFrag text_;

    /**
     * Stores the values of the annotation item's attributes.
     * The value of the n'th attribute is stored in the n'th cell of the vector.
     * Other cells are empty.
     */
    std::vector<zvalue> values_;

    /**
     * The value of attributes_[n] indicates whether the value of the n'th attribute
     * is present in the values_ vector.
     */
    std::bitset<128> attributes_;

    size_t resize_(size_t size);
    bool areAttributesTheSame_(const AnnotationItem& other) const;
};

