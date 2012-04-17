#include <typeinfo>
#include <cstdarg>
#include <iostream>

#include <string>
#include "FtpException.h"
#include "Settings.h"

using namespace std;

FtpException::FtpException(string message, ...) : exception() {
    va_list args;
    va_start(args, message);
    char buffer[Settings::MESSAGE_LENGTH];
    vsprintf_s(buffer, message.c_str(), args);
    va_end(args);
    message_ = string(buffer);
}

FtpException::~FtpException() {

}

void FtpException::what() {
    printf(message_.c_str());
}