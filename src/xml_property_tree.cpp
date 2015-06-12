#include "xml_property_tree.hpp"

#include <boost/property_tree/xml_parser.hpp>

XmlPropertyTree::XmlPropertyTree(std::istream& inputStream) {
    boost::property_tree::read_xml(
        inputStream,
        (boost::property_tree::ptree&)*this,
        boost::property_tree::xml_parser::no_comments|
        boost::property_tree::xml_parser::trim_whitespace|
        boost::property_tree::xml_parser::no_concat_text);
}

XmlPropertyTree::XmlPropertyTree(const boost::filesystem::path& xmlFilePath) {
    boost::property_tree::read_xml(
        xmlFilePath.string(),
        (boost::property_tree::ptree&)*this,
        boost::property_tree::xml_parser::no_comments|boost::property_tree::xml_parser::trim_whitespace);
}

void XmlPropertyTree::write(std::ostream& outputStream) {
    boost::property_tree::write_xml(
        outputStream,
        (boost::property_tree::ptree&)*this,
        boost::property_tree::xml_parser::xml_writer_settings<char>(' ', 2)
        );
}
