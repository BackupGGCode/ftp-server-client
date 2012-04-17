#include <fstream>
#include <algorithm>
#include <cctype>
#include "FileParser.h"
#include "Settings.h"
#include "FtpException.h"
#include "TextRepository.h"

FileParser::FileParser(string fileSrc) {
    fileSrc_ = fileSrc;
}

FileParser::~FileParser() {
}

void FileParser::parseFile() {
    FILE* file;

    int ret = fopen_s(&file, fileSrc_.c_str(), "rb");
    if (ret > 0) {
        throw FtpException(TextRepository::APP_ERROR_FILENOTFOUND, fileSrc_.c_str());
    }
    bool isInComment = false;
    bool isInValue = false;
    string key = "";
    string value = "";
    while (!feof(file)) {
        char ch = fgetc(file);
        if (!isInComment) {
            if(ch == COMMENT) {
                isInComment = true;
            } else {
                if (ch == NEW_LINE) {
                    if (key.length() && value.length()) {
                        fields_.insert(pair<string,string>(string(key),string(value)));
                    }
                    key.clear();
                    value.clear();
                    isInValue = false;
                } else {
                    if (!isspace(ch)) {
                        if (isInValue) {
                            value.append(1, ch);
                        } else {
                            if(ch == SEPARATOR) {
                                isInValue = true;
                            } else {
                                key.append(1, ch);
                            }
                        }
                    }
                }
            }
        } else {
            if (ch == NEW_LINE) {
                if (key.length() && value.length()) {
                    fields_.insert(pair<string,string>(string(key),string(value)));
                }
                isInComment = false;
            }
        }
    }

    if (key.length() && value.length()) {
        fields_.insert(pair<string,string>(string(key),string(value)));
    }
 
    fclose(file);
}

int FileParser::getInt(string fieldName) {
    return atoi(getString(fieldName).c_str());
}

string FileParser::getString(string fieldName) {
    for (map<string,string>::iterator it = fields_.begin(); it != fields_.end(); it++) {
        if (!(*it).first.compare(fieldName)) {
            return (*it).second.c_str();
        }
    }
    throw FtpException(TextRepository::APP_ERROR_INVALIDPARAM, fieldName.c_str(), fileSrc_.c_str());
}

bool FileParser::getBool(string fieldName) {
    return !getString(fieldName).compare("true") ? true : false;
}

list<string> FileParser::getParameters() {
    list<string> parameters;
    for (map<string,string>::iterator it = fields_.begin(); it != fields_.end(); it++) {
        parameters.push_back((*it).first);
    }
    return parameters;
}