#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <sstream>

#include <string>
#include "Session.h"
#include "Settings.h"
#include "FtpException.h"

using namespace std;
Session::Session(int socket, string & recvError, string & sendError) {
    socket_ = socket;
    recvError_ = recvError;
    sendError_ = sendError;
}


Session::~Session(void){
}

Command Session::recvMessage() {
    char reply[Settings::BUFFER_LENGTH];
    int ret = recv(socket_, reply, Settings::BUFFER_LENGTH, 0);
    if(ret == SOCKET_ERROR) {
        throw FtpException(recvError_, socket_);
    }
    if(ret != Settings::BUFFER_LENGTH) {
        reply[ret] = '\0';
    }
    return Command(string(reply));
}

void Session::recvMessageBin(FILE *file) {
    char buffer[Settings::BUFFER_LENGTH_BIN];
    int ret = recv(socket_, buffer, Settings::BUFFER_LENGTH_BIN, 0);   
    if(ret == SOCKET_ERROR) {
        throw FtpException(recvError_, socket_);
    }
    fwrite(buffer, ret, 1, file);
    if(ret != Settings::BUFFER_LENGTH_BIN) {
        //reply[ret] = '\0';
    }
}

void Session::sendMessage(Command command) {
    string message = command.getCommand();
    int ret = send(socket_, message.c_str(), message.length(), 0);
    if(ret == SOCKET_ERROR) {
        throw FtpException(sendError_);
    }
}

void Session::sendMessageBin(FILE* file, int bufferSize) {
    char buffer[Settings::BUFFER_LENGTH_BIN];
    fread(buffer, 1, bufferSize, file);
    int ret = send(socket_, buffer, bufferSize, 0);
    if (ret == SOCKET_ERROR) {
        throw FtpException(sendError_);
    }
}
