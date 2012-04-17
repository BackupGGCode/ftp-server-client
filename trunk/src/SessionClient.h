#pragma once
#include <iostream>
#include <fstream>

#include <string>
#include "Command.h"
#include "Location.h"
#include "Session.h"
#include "Client.h"

using namespace std;

class SessionClient : public Session
{
public:
    SessionClient(Client* client);
    ~SessionClient();
    void start();
	void startBackup();
    void login();
	void loginBackup();
    void processCommand(Command command, Command executedCommand);
    void getFile(Command command, Command executedCommand);
    void sendFile(Command command, Command executedCommand);
    list<string> getFileList(Location location, string dir = "");
private:
    Client* client_;
    Location location_;
};