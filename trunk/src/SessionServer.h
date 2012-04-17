#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include "Session.h"

#include "Location.h"
#include "Command.h"
#include "User.h"

using namespace std;

class Server;

class SessionServer : public Session
{
public:
    SessionServer(int connectionID);
    ~SessionServer();
    int getConnectionId();
    User getUser();
    void start();
    void login();
    void close();
    string getTimeLogon();
    string getTimeLastActivity();
private:
    int connectionId_;
    Location location_;
    User user_;
    string timeLogon_;
    string timeLastActivity_;
    void sendFileList();
    void sendFile(Command command);
    void recvFile(Command command);
    void createDirectory(Command command);
    void removeFile(Command command);
    void changeDirectory(Command command);
    void doWork(Command response);
    void sendOnlineList();
};