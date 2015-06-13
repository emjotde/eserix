#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <boost/program_options.hpp>

#include "srx_segmenter.hpp"

int main(int argc, char** argv) {
    
    namespace po = boost::program_options;
    po::options_description options("SRX sentence segmenter");

    std::string language, rules;
    std::size_t soft_limit, hard_limit;
    
    options.add_options()
      ("help,h", po::bool_switch(), "Show this help message")
      //("language,l", po::value<std::string>(&language)->default_value("en"),
      // "Language in SRX file")
      ("rules,r", po::value<std::string>(&rules)->default_value("srx/rules.srx"),
       "Path to SRX file")
      ("soft_limit", po::value<std::size_t>(&soft_limit)->default_value(0),
       "Break at next space after  args  bytes, disabled by default")
      ("hard_limit", po::value<std::size_t>(&hard_limit)->default_value(0),
       "Break after  args  bytes, disabled by default")
      ("trim,t", po::bool_switch(),
       "Trim leading and trailing white spaces")
    ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);
    
    if(vm["help"].as<bool>()) {
        std::cerr << options << std::endl;
        return 1;
    }
    po::notify(vm);
    bool doTrim = vm["trim"].as<bool>();
    
    std::map<std::string, std::shared_ptr<SrxSegmenter>> langSrx;
    
    XmlPropertyTree xml(std::cin);
    typedef XmlPropertyTree::path_type path;
    for(auto& v : xml.get_child(path("TEI.2/text/body", '/'))) {
        if(v.first == "p") {
            std::string id = v.second.get_child("<xmlattr>.id").data();
            std::string lang = v.second.get_child("<xmlattr>.lang").data();
            std::string text = v.second.get_child("<xmltext>").data();
            
            v.second.erase("<xmltext>");
            v.second.erase("<xmlattr>");
            v.second.add("<xmlattr>.id", id);
            
            if(langSrx.count(lang) == 0)
                langSrx[lang] = std::shared_ptr<SrxSegmenter>(
                    new SrxSegmenter(lang, rules, hard_limit, soft_limit));
            
            SrxSentenceCutter srxCutter(*langSrx[lang], text, doTrim);
            std::string frag;
            size_t subid = 1;
            while(srxCutter >> frag) {    
                auto& s = v.second.add("s", frag);
                s.add("<xmlattr>.id", id + ":" + boost::lexical_cast<std::string>(subid++));
                s.add("<xmlattr>.lang", lang);
            }
        }
    }
    xml.write(std::cout);
    return 0;
}
