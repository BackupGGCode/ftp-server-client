#pragma once

#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <list>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <map>
#include <string>

#include "Location.h"

class SessionServer;

using namespace std;

class Server
{
public:
    static Server* getInstance();
    void        init();
    void        listen();
                ~Server();
    list<string> getFileList(Location location, bool fileSize);
    int         getFileSize(Location location, string fileName);
    FILE*       getFile(Location location, string fileName);
    FILE*       createFile(Location location, string fileName);
    bool        directoryExsists(Location location);
    void        removeFile(Location location, string fileName);
    void        createDirectory(Location location, string fileName);
    void        removeSession(int connectionId);
    list<string> getOnlineList();
    static string getServerTime();
    static string getServerTimeCompressed();
protected:
private:
                Server();
    WSADATA     wsda_;
    DWORD*      threads_;
    SOCKADDR_IN addrLocal_;
    int         listener_;
    map<int, SessionServer*> sessions_;
    static      DWORD WINAPI userThread(PVOID args);
    static Server* instance_;
};