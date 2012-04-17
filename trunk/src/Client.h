#pragma once

#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <string>
#include "Location.h"
using namespace std;

class Client
{
public:
    static Client* getInstance();
                ~Client();
    void        init();
    void        connect();
    void        console();
	void        backup();
    SOCKET      getSocket();
    int         getFileSize(string fileName);
    FILE*       getFile(string fileName);
protected:
private:
                Client();
    WSADATA     wsda_;
    DWORD*      threads_;
    SOCKADDR_IN addrRemote_;
    SOCKET         listener_;
    static Client* instance_;
};