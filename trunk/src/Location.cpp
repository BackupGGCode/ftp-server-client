#include <list>
#include <string>
#include "Location.h"

using namespace std;
Location::Location() {
    fileName_ = "";
}

Location::Location(string location) {
    fileName_ = "";
    parseString(location);
}

Location::~Location() {
}

string Location::getLocation() {
    string currentLocation;
    for (list<string>::iterator it = location_.begin(); it != location_.end(); it++) {
        currentLocation.append(*it);
        currentLocation.append("/");
    }
    currentLocation.append(fileName_);
    return currentLocation;
}

string Location::getFileName() {
    return fileName_;
}

 void Location::setFileName(string fileName) {
     fileName_ = fileName;
 }

void Location::goBack() {
    if(location_.size() > 0) {
        location_.pop_back();
    }
}

void Location::goDirectory(string directory) {
    if (!directory.compare("..")) {
        goBack();
    } else if (directory.compare(".") && !directory.empty()) {
        location_.push_back(string(directory));
    }
}

void Location::goPath(string directory) {
    directory.append("/");
    parseString(directory);
}

void Location::goHome() {
    location_.clear();
}

void Location::parseString(string location) {
    if (!location.empty() && location.at(0) == '/') {
        goHome();
    }
    string subString;
    for (unsigned int i = 0; i < location.length(); i++) {
        if (location.at(i) == '/') {
            goDirectory(subString);
            subString.clear();
        } else {
            subString.append(1, location.at(i));
        }
    }
    //goDirectory(subString);
    fileName_ = subString;
}