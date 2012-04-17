#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <string>
#include <cmath>

#include "Server.h"
#include "Settings.h"
#include "FtpException.h"
#include "TextRepository.h"
#include "SessionServer.h"

using namespace std;

Server* Server::instance_ = NULL;

Server* Server::getInstance() {
    if (!instance_) {
        instance_ = new Server();
    }
    return instance_;
}

Server::Server() {
    threads_ = new DWORD[Settings::getMaxThreads()];
}

Server::~Server(){
    WSACleanup();
}

void Server::init() {
    WSAStartup(MAKEWORD(2,2), &wsda_);
    listener_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (listener_ == SOCKET_ERROR) {
		throw FtpException(TextRepository::SERVER_ERROR_SOCKET);
	}

	addrLocal_.sin_family = AF_INET;
	addrLocal_.sin_port = htons(Settings::getServerPort());
	addrLocal_.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(listener_, (struct sockaddr *) &addrLocal_, sizeof(addrLocal_));

	if (ret == SOCKET_ERROR) {
        throw FtpException(TextRepository::SERVER_ERROR_BIND);
	}
}

void Server::listen() {
	int ret = ::listen(listener_, 10);
	if (ret == SOCKET_ERROR) {
        throw FtpException(TextRepository::SERVER_ERROR_LISTEN);
	}

    SOCKADDR_IN addrRemote;
    SessionServer *sessionServer;

    int addrRemoteLen = sizeof(addrRemote);

    TextRepository::printMessage(TextRepository::SERVER_STARTED);
	TextRepository::printMessage(TextRepository::SERVER_LISTEN, Settings::getServerPort());
    int connectionId;
	for (;;) {
		connectionId = accept(listener_, (struct sockaddr *) &addrRemote, &addrRemoteLen);
        sessionServer = new SessionServer(connectionId);
        sessions_.insert(pair<int, SessionServer*>(connectionId, sessionServer));
		TextRepository::printMessage(TextRepository::SERVER_NEW_CONNECTION, connectionId, inet_ntoa(addrRemote.sin_addr));
		CreateThread(NULL, 0, Server::userThread, (PVOID)sessionServer, 0, &threads_[0]);
	}
}

DWORD WINAPI Server::userThread(PVOID args) {
    SessionServer* sessionServer = static_cast<SessionServer*>(args);
    try {
        sessionServer->login();
        sessionServer->start();
    } catch (FtpException e) {
        e.what();
    }
    Server::getInstance()->removeSession(sessionServer->getConnectionId());
    delete sessionServer;
    return NULL;
}

list<string> Server::getFileList(Location location, bool fileSize) {
    list<string> fileList;
    WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
    string path;
    path.append(Settings::getServerDir());
    path.append(location.getLocation());
    path.append(1, TextRepository::SLASH);
    path.append(1, TextRepository::MATCHALL);
    hFind = FindFirstFileA(path.c_str(), &FindFileData);
    do {
        if (location.getLocation().empty() && !static_cast<string>(FindFileData.cFileName).compare("..")) {
            continue;
        }
        
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            char buffer[255];
            sprintf_s(buffer, 255, TextRepository::CLIENT_LSDIR_PATTERN.c_str(), TextRepository::SPACE, static_cast<string>(FindFileData.cFileName).c_str());
            fileList.push_back(buffer);
        } else {
            char buffer[255];
			if(fileSize) {
				LARGE_INTEGER fileSize;
				fileSize.LowPart = FindFileData.nFileSizeLow;
				fileSize.HighPart = FindFileData.nFileSizeHigh;
				int fileSizeInt;
				string fileSizeUnit;

				const int CONVERTER = 1024;
				const long long KB_CONVERTER = CONVERTER;
				const long long MB_CONVERTER = KB_CONVERTER*CONVERTER;
				const long long GB_CONVERTER = MB_CONVERTER*CONVERTER;
				const long long TB_CONVERTER = GB_CONVERTER*CONVERTER;
				if (fileSize.QuadPart < KB_CONVERTER) {
					fileSizeUnit = TextRepository::UNIT_B;
					fileSizeInt = static_cast<int>(fileSize.QuadPart);
				} else if(fileSize.QuadPart < MB_CONVERTER) {
					fileSizeUnit = TextRepository::UNIT_KB;
					fileSizeInt = static_cast<int>(fileSize.QuadPart/KB_CONVERTER);
				} else if(fileSize.QuadPart < GB_CONVERTER) {
					fileSizeUnit = TextRepository::UNIT_MB;
					fileSizeInt = static_cast<int>(fileSize.QuadPart/MB_CONVERTER);
				} else if(fileSize.QuadPart < TB_CONVERTER) {
					fileSizeUnit = TextRepository::UNIT_GB;
					fileSizeInt = static_cast<int>(fileSize.QuadPart/GB_CONVERTER);
				} else {
					fileSizeUnit = TextRepository::UNIT_TB;
					fileSizeInt = static_cast<int>(fileSize.QuadPart/TB_CONVERTER);
				}
				sprintf_s(buffer, 255, TextRepository::CLIENT_LS_PATTERN.c_str(), fileSizeInt, fileSizeUnit.c_str(), static_cast<string>(FindFileData.cFileName).c_str());
			} else {
				sprintf_s(buffer, 255, "%s", static_cast<string>(FindFileData.cFileName).c_str());
			}
            fileList.push_back(string(buffer));
        }
        
    } while (FindNextFileA(hFind, &FindFileData) != 0);

    FindClose(hFind);
    return fileList;
}

FILE* Server::getFile(Location location, string fileName) {
    string path;
    path.append(Settings::getServerDir());
    path.append(location.getLocation());
    path.append(fileName);

    FILE* file;
    int ret = fopen_s(&file, path.c_str(), "rb");
    if(ret > 0) {
        throw FtpException(TextRepository::SERVER_ERROR_FILENOTFOUND, path.c_str());
    }
    return file;
}

FILE* Server::createFile(Location location, string fileName) {
    string path;
    path.append(Settings::getServerDir());
    path.append(location.getLocation());
    path.append(fileName);

    FILE* file;
    int ret = fopen_s(&file, path.c_str(), "wb");
    if(ret > 0) {
        throw FtpException(TextRepository::SERVER_ERROR_FILECREATION, path.c_str());
    }
    return file;
}

bool Server::directoryExsists(Location location) {
    string path;
    path.append(Settings::getServerDir());
    path.append(location.getLocation());
    path.append(1, TextRepository::SLASH);
    path.append(1, TextRepository::MATCHALL);

    WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
    bool directoryExsists;
    hFind = FindFirstFileA(path.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)  {
        directoryExsists = false;
    } else {
        directoryExsists = true;
    }
    FindClose(hFind);
    return directoryExsists;
}

int Server::getFileSize(Location location, string fileName) {
    string path;
    path.append(Settings::getServerDir());
    path.append(location.getLocation());
    path.append(fileName);
    ifstream file(path.c_str(), ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        throw FtpException(TextRepository::SERVER_ERROR_FILENOTFOUND, path.c_str());
    }
	int fileSize = static_cast<int>(file.tellg());
	file.close();
    return fileSize;
}

void Server::removeFile(Location location, string fileName) {
    string path;
    path.append(Settings::getServerDir());
    path.append(location.getLocation());
    path.append(fileName);
    if(remove(path.c_str()) != 0) {
        throw FtpException(TextRepository::SERVER_ERROR_FILENOTFOUND, path.c_str());
    }
}

void Server::createDirectory(Location location, string fileName) {
    string path;
    path.append(Settings::getServerDir());
    path.append(location.getLocation());
    path.append(fileName);
    if(_mkdir(path.c_str()) != 0) {
        throw FtpException(TextRepository::SERVER_ERROR_CREATEDIR, path.c_str());
    }
}

void Server::removeSession(int connectionId) {
    sessions_.erase(connectionId);
}

list<string> Server::getOnlineList() {
    list<string> onlineUsers;
    char buffer[255];
    sprintf_s(buffer, 255, TextRepository::CLIENT_WHO_LABELSPATTERN.c_str(), 
                           TextRepository::CLIENT_WHO_LABEL_ID.c_str(), 
                           TextRepository::CLIENT_WHO_LABEL_USERNAME.c_str(), 
                           TextRepository::CLIENT_WHO_LABEL_TIMELOGON.c_str(), 
                           TextRepository::CLIENT_WHO_LABEL_TIMELASTACTIV.c_str()
                           );
    onlineUsers.push_back(string(buffer));
    onlineUsers.push_back("");
    for(map<int, SessionServer*>::iterator it = sessions_.begin(); it != sessions_.end(); it++) {
        string userName = (*it).second->getUser().getName();
        if (!userName.empty()) {
            char buffer[255];

            SessionServer* sessionServer = (*it).second;
            int connectionId = (*it).first;
            string userName = sessionServer->getUser().getName();
            string timeLogon = sessionServer->getTimeLogon();
            string timeLastActivity = sessionServer->getTimeLastActivity();
            sprintf_s(buffer, 255, TextRepository::CLIENT_WHO_PATTERN.c_str(), 
                                   connectionId, 
                                   userName.c_str(), 
                                   timeLogon.c_str(), 
                                   timeLastActivity.c_str()
                                   );
            string row;
            row.append(buffer);
            onlineUsers.push_back(row);
        }
    }
    return onlineUsers;
}

string Server::getServerTime() {
    SYSTEMTIME sysTime;
    GetSystemTime(&sysTime);
    char buffer[255];
    sprintf_s(buffer, 255, TextRepository::CLIENT_DATETIME_PATTERN.c_str(), sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
    return string(buffer);
}

string Server::getServerTimeCompressed() {
    SYSTEMTIME sysTime;
    GetSystemTime(&sysTime);
    char buffer[255];
    sprintf_s(buffer, 255, TextRepository::CLIENT_DATETIME_COMPRESSED_PATTERN.c_str(), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
    return string(buffer);
}