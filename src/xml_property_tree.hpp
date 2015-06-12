#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>

/**
 * A simple wrapper for Boost property tree. Use for parsing XMLs.
 */
class XmlPropertyTree : public boost::property_tree::ptree {

public:
    XmlPropertyTree(std::istream& inputStream);
    XmlPropertyTree(const boost::filesystem::path& xmlFilePath);

    void write(std::ostream& outputStream);
};
