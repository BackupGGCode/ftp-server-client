#pragma once

#include <string>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <sstream>
#include "Command.h"
using namespace std;

class Session
{
public:
    Session(int socket, string & recvError, string & sendError);
    ~Session();
    void sendMessage(Command message);
    void sendMessageBin(FILE *file, int bufferSize);
    Command recvMessage();
    void recvMessageBin(FILE *file);
protected:
private:
    int socket_;
    string recvError_;
    string sendError_;
};