#include "psi_exception.hpp"

PsiException::PsiException(const std::string& msg):msg_(msg) {
}

PsiException::~PsiException() throw() {
}

const char* PsiException::what() const throw() {
    return msg_.c_str();
}
