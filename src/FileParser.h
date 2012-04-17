#pragma once

#include <string>
#include <map>
#include <list>

using namespace std;

class FileParser
{
public:
    static const char SEPARATOR = '=';
    static const char COMMENT = '#';
    static const char NEW_LINE = '\n';

    FileParser(string fileSrc);
    ~FileParser();
    void parseFile();
    int getInt(string fieldName);
    string getString(string fieldName);
    bool getBool(string fieldName);
    list<string> getParameters();
private:
    string fileSrc_;

    map<string, string> fields_;
};