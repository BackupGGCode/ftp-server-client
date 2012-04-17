#pragma once

#include <string>
#include <list>
#include <iostream>
#include <string>

using namespace std;

class Command
{
public:
    Command(string command);
    Command() {}
    ~Command();
    string getName();
    void setName(string name);
    string getArgument(unsigned int id);
    void addArg(string arg);
    void clearArgs();
    void clearLastArg();
    string getCommand();
private:
    string name_;
    list<string> args_;
};