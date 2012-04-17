#pragma once
#include <typeinfo>
#include <cstdarg>
#include <iostream>
#include <string>

using namespace std;

class FtpException : public exception
{
public:
    FtpException() : exception() {}
    FtpException(string message, ...);
    ~FtpException();
    void    what();
private:
    string message_;
};