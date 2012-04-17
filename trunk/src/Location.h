#pragma once

#include <list>
#include <string>

using namespace std;

class Location
{
public:
    Location(string location);
    Location();
    ~Location();
    string getLocation();
    string getFileName();
    void setFileName(string fileName);
    void goBack();
    void goDirectory(string directory);
    void goPath(string directory);
    void goHome();
private:
    list<string> location_;
    string fileName_;
    void parseString(string location);
};