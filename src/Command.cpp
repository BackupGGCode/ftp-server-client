#include <list>
#include <iostream>
#include <string>
#include "Command.h"

using namespace std;

Command::Command(string command) {
    string subString;
    bool setArgs = false;
    bool isOpen = false;
    for(unsigned int i = 0; i < command.length(); i++) {
        if(command.at(i) == '"') {
            isOpen = !isOpen;
            continue;
        }
        if (command.at(i) == ' ' && !isOpen) {
            if (setArgs) {
                args_.push_back(string(subString));
            } else {
                name_ = string(subString);
                setArgs = true;
            }
            subString.clear();
        } else {
            subString.append(1, command.at(i));
        }
    }
    if (setArgs) {
        args_.push_back(string(subString));
    } else {
        name_ = string(subString);
    }
}

Command::~Command() {
}

string Command::getName() {
    return name_;
}

string Command::getArgument(unsigned int id) {
    int n = 0;
    for (list<string>::iterator it = args_.begin(); it != args_.end(); it++) {
        if (n == id) {
            return *it;
        }
        n++;
    }
    return "";
}

void Command::addArg(string arg) {
    args_.push_back(arg);
}

void Command::clearArgs() {
    args_.clear();
}

void Command::clearLastArg() {
    args_.pop_back();
}

string Command::getCommand() {
    string command;
    command.append(name_);
    bool containsSpace = false;
    for (list<string>::iterator it = args_.begin(); it != args_.end(); it++) {
        command.append(" ");
        containsSpace = false;
        if ((*it).find(' ') != string::npos) {
            command.append(1, '"');
            containsSpace = true;
        }
        command.append(*it);
        
        if(containsSpace) {
            command.append(1, '"');
        }
    }
    return command;
}

void Command::setName(string name) {
    name_ = name;
}