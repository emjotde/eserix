#include <iostream>

#include "srx_segmenter.hpp"

int main(int argc, char** argv) {
    
    SrxSegmenter srxSeg(argv[2], argv[1], 0, 0);
    SrxSentenceCutter srxCut(srxSeg);
    
    std::string line;
    while(std::getline(std::cin, line)) {
        srxCut.reset();
        size_t pos = 0;
        while(pos != std::string::npos) {
            AnnotationItem a = srxCut.cutOff(line, pos);
            std::cout << a.getText() << std::endl;
        }
    }
    return 0;
}
