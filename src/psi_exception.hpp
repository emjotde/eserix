#pragma once

#include <exception>
#include <string>

class PsiException : public std::exception {
public:
    PsiException(const std::string& msg);
    virtual const char* what() const throw();
    virtual ~PsiException() throw();

private:
    std::string msg_;
};

