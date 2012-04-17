#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <string>
#include "Client.h"
#include "FtpException.h"
#include "Settings.h"
#include "TextRepository.h"
#include "SessionClient.h"

using namespace std;

Client* Client::instance_ = NULL;

Client* Client::getInstance() {
    if (!instance_) {
        instance_ = new Client();
    }
    return instance_;
}

Client::Client(){
}

Client::~Client(){
    WSACleanup();
}

void Client::init() {
    WSAStartup(MAKEWORD(2,2), &wsda_);
    listener_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (listener_ == SOCKET_ERROR) {
		throw FtpException(TextRepository::CLIENT_ERROR_SOCKET);
	}
	addrRemote_.sin_family = AF_INET;
	addrRemote_.sin_port = htons(Settings::getServerPort());
	addrRemote_.sin_addr.s_addr = inet_addr(Settings::getServerIP());

	if (addrRemote_.sin_addr.s_addr == INADDR_NONE || addrRemote_.sin_addr.s_addr == INADDR_ANY) {
        struct hostent *host;
		host = gethostbyname(Settings::getServerIP());
		if (host == NULL) {
			throw FtpException(TextRepository::CLIENT_ERROR_DNS);
		}
		memcpy(&addrRemote_.sin_addr, host->h_addr_list[0], host->h_length);
	}
}

void Client::connect() {
    TextRepository::printMessage(TextRepository::CLIENT_CONNECT, Settings::getServerIP(), Settings::getServerPort());
	int ret = ::connect(listener_, (struct sockaddr *) &addrRemote_, sizeof(addrRemote_));
	if (ret == SOCKET_ERROR) {
		throw FtpException(TextRepository::CLIENT_ERROR_CONNECT);
	}
    TextRepository::printMessage(TextRepository::CLIENT_CONNECT_SUCCESS);
}

void Client::console() {
    TextRepository::printMessage(TextRepository::CLIENT_CONSOLE);
    SessionClient* sessionClient = new SessionClient(this);
    sessionClient->login();
    sessionClient->start();
    delete sessionClient;
}

void Client::backup() {
    TextRepository::printMessage(TextRepository::CLIENT_CONSOLE);
    SessionClient* sessionClient = new SessionClient(this);
    sessionClient->loginBackup();
    sessionClient->startBackup();
    delete sessionClient;
}

int Client::getFileSize(string fileName) {
    string path;
    path.append(Settings::getClientDir());
    path.append(fileName);
    ifstream file(path.c_str(), ios::in|ios::binary|ios::ate);
    if (!file.is_open()) {
        throw FtpException(TextRepository::CLIENT_FILENOTFOUND);
    }
	int fileSize = static_cast<int>(file.tellg());
	file.close();
    return fileSize;
}

SOCKET Client::getSocket() {
    return listener_;
}

FILE* Client::getFile(string fileName) {
    string path;
    path.append(Settings::getClientDir());
    path.append("/");
    path.append(fileName);

    FILE* file;
    int ret = fopen_s(&file, path.c_str(), "rb");
    if(ret > 0) {
        throw FtpException(TextRepository::CLIENT_ERROR_FILENOTFOUND, path.c_str());
    }
    return file;
}