#include <iostream>
#include <sstream>
#include <string>
#include "SessionServer.h"
#include "TextRepository.h"
#include "FtpException.h"
#include "Settings.h"
#include "Server.h"

using namespace std;

SessionServer::SessionServer(int connectionID) : Session(connectionID, TextRepository::SERVER_ERROR_RECV, TextRepository::SERVER_ERROR_SEND) {
    connectionId_ = connectionID;
}

SessionServer::~SessionServer() {
}

void SessionServer::start() {
    for(;;) {
        Command command = recvMessage();
        timeLastActivity_ = Server::getInstance()->getServerTime();
        doWork(command);
    }
}

void SessionServer::close() {
    sendMessage(TextRepository::CLIENT_COMMAND_EXIT);
    throw FtpException(TextRepository::SERVER_DISCONNECT, connectionId_);
}

void SessionServer::doWork(Command command) {
    TextRepository::printMessage(TextRepository::SERVER_EXECUTE_COMMAND, connectionId_, command.getCommand().c_str());
    if (!command.getName().compare(TextRepository::CLIENT_COMMAND_LS)) {
        sendFileList();
    } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_CD)) {
        changeDirectory(command);
    } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_GET)) {
        sendFile(command);
    } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_SEND)) {
        recvFile(command);
    } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_RM)) {
        removeFile(command);
    } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_MKDIR)) {
        createDirectory(command);
    } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_EXIT)) {
        close();
    } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_WHO)) {
        sendOnlineList();
    } else {
        Command response(TextRepository::SERVER_COMMAND_STOP);
        response.addArg(TextRepository::CLIENT_UNKNOWN_COMMAND);
        sendMessage(response);
    }
}

void SessionServer::sendFileList() {
    if (user_.getGrantRead()) {
        Command response;
        Command command;
        list<string> fileList = Server::getInstance()->getFileList(location_, true);
        response.setName(TextRepository::SERVER_COMMAND_PRINT);
        string fileListStr;
        for (list<string>::iterator it = fileList.begin(); it != fileList.end(); it++) {
            response.clearArgs();
            response.addArg(*it);
            sendMessage(response);
            command = recvMessage();
            if (command.getName().compare(TextRepository::CLIENT_COMMAND_OK)) {
                break;
            }
        }
        sendMessage(Command(TextRepository::SERVER_COMMAND_STOP));
    } else {
        Command error(TextRepository::SERVER_COMMAND_STOP);
        error.addArg(TextRepository::CLIENT_NOPRIVILEGES);
        sendMessage(error);
    }

}

void SessionServer::sendFile(Command command) {
    Command responseStop(TextRepository::SERVER_COMMAND_STOP);
    if (user_.getGrantRead()) {
        if (command.getArgument(0).empty()) {
            responseStop.addArg(TextRepository::CLIENT_MISSING_ARGUMENT);
        } else {
            try {
                FILE* file = Server::getInstance()->getFile(location_, command.getArgument(0));
                int fileSize = Server::getInstance()->getFileSize(location_, command.getArgument(0));
                Command fileSizeCommand(TextRepository::CLIENT_COMMAND_GET);

                fileSizeCommand.addArg(TextRepository::intToString(fileSize));
                sendMessage(fileSizeCommand);
                Command response = recvMessage();
                if(!response.getName().compare(TextRepository::CLIENT_COMMAND_OK)) {
			        while(fileSize > 0) {
				        if(fileSize > Settings::BUFFER_LENGTH_BIN) {
                            sendMessageBin(file, Settings::BUFFER_LENGTH_BIN);
                            if (recvMessage().getName().compare(TextRepository::CLIENT_COMMAND_OK)) {
                                throw FtpException(TextRepository::SERVER_ERROR_RECV);
                            }
				        } else {
                            sendMessageBin(file, fileSize);
                            if (recvMessage().getName().compare(TextRepository::CLIENT_COMMAND_OK)) {
                                throw FtpException(TextRepository::SERVER_ERROR_RECV);
                            }
				        }
				        fileSize -= Settings::BUFFER_LENGTH_BIN;
			        }
                    fclose(file);
                    responseStop.addArg(TextRepository::CLIENT_FILE_SAVED);
                } else {
                    responseStop.addArg(TextRepository::CLIENT_TERMINATED);
                }
            } catch(FtpException e) {
                e.what();
                responseStop.addArg(TextRepository::CLIENT_FILENOTFOUND);
            }
        }
    } else {
        responseStop.addArg(TextRepository::CLIENT_NOPRIVILEGES);
    }
    sendMessage(responseStop);
}

void SessionServer::recvFile(Command command) {
    Command responseStop(TextRepository::SERVER_COMMAND_STOP);

    if (user_.getGrantWrite()) {
        if (command.getArgument(0).empty()) {
            responseStop.addArg(TextRepository::CLIENT_MISSING_ARGUMENT);
        } else {
            sendMessage(Command(TextRepository::CLIENT_COMMAND_SEND));
            Command response = recvMessage();
        
            if (!response.getName().compare(TextRepository::CLIENT_COMMAND_GET)) {
                int filesize = atoi(response.getArgument(0).c_str());
                string fileName = command.getArgument(0);
                FILE* file = Server::getInstance()->createFile(location_, fileName);
                sendMessage(Command(TextRepository::SERVER_COMMAND_OK));
		        while (filesize > 0) {
			        if (filesize >= Settings::BUFFER_LENGTH_BIN) {
                        recvMessageBin(file);
                        sendMessage(Command(TextRepository::SERVER_COMMAND_OK));
			        } else {
                        recvMessageBin(file);
                        sendMessage(Command(TextRepository::SERVER_COMMAND_OK));
			        }
			        filesize -= Settings::BUFFER_LENGTH_BIN;
		        }
                fclose(file);
                recvMessage();
                responseStop.addArg(TextRepository::CLIENT_FILE_SAVED);
            }
        }
    } else {
        responseStop.addArg(TextRepository::CLIENT_NOPRIVILEGES);
    }
    sendMessage(responseStop);
}

void SessionServer::changeDirectory(Command command) {
    Command responseStop(TextRepository::SERVER_COMMAND_STOP);
    if (user_.getGrantRead()) {
        string path = command.getArgument(0);
        if (!path.empty()) {
            Location location(location_);
            location.goPath(path);
            if (Server::getInstance()->directoryExsists(location)) {
                Command response(TextRepository::CLIENT_COMMAND_CD);
                response.addArg(path);
                sendMessage(response);
                if(!recvMessage().getName().compare(TextRepository::CLIENT_COMMAND_OK)) {
                    location_ = location;
                } else {
                    responseStop.addArg(TextRepository::CLIENT_TERMINATED);
                }
            } else {
                responseStop.addArg(TextRepository::CLIENT_DIR_NOT_FOUND);
            }
        }
    } else {
        responseStop.addArg(TextRepository::CLIENT_NOPRIVILEGES);
    }
    sendMessage(responseStop);
}

void SessionServer::createDirectory(Command command) {
    Command responseStop(TextRepository::SERVER_COMMAND_STOP);
    if (user_.getGrantWrite()) {
        string dirName = command.getArgument(0);
        if (!dirName.empty()) {
            try {
                Server::getInstance()->createDirectory(location_, dirName);
            } catch(FtpException e) {
                responseStop.addArg(TextRepository::CLIENT_CREATEDIR);
            }
        } else {
            responseStop.addArg(TextRepository::CLIENT_MISSING_ARGUMENT);
        }
    } else {
        responseStop.addArg(TextRepository::CLIENT_NOPRIVILEGES);
    }
    sendMessage(responseStop);
}

void SessionServer::removeFile(Command command) {

    Command responseStop(TextRepository::SERVER_COMMAND_STOP);
    if (user_.getGrantWrite()) {
        string fileName = command.getArgument(0);
        if (!fileName.empty()) {
            try {
                Server::getInstance()->removeFile(location_, fileName);
            } catch(FtpException e) {
                responseStop.addArg(TextRepository::CLIENT_REMOVEFILE);
            }
        } else {
            responseStop.addArg(TextRepository::CLIENT_MISSING_ARGUMENT);
        }
    } else {
        responseStop.addArg(TextRepository::CLIENT_NOPRIVILEGES);
    }
    sendMessage(responseStop);
}

int SessionServer::getConnectionId() {
    return connectionId_;
}

void SessionServer::login() {
    if (!Settings::getIsAnonymous()) {
        sendMessage(Command(TextRepository::CLIENT_COMMAND_LOGIN));
        while(true) {
            Command response = recvMessage();
            if (!response.getName().compare(TextRepository::CLIENT_COMMAND_LOGIN)) {
                string login = response.getArgument(0);
                string passwd = response.getArgument(1);
                User::reloadUserRepository();
                try {
                    user_ = User::getUserFromRepository(login, passwd, connectionId_);
                    sendMessage(Command(TextRepository::SERVER_COMMAND_OK));
                    TextRepository::printMessage(TextRepository::SERVER_LOGGED, connectionId_, login.c_str());
                    break;
                } catch(FtpException e) {
                    e.what();
                    sendMessage(Command(TextRepository::SERVER_COMMAND_STOP));
                }
            }
        }
    } else {
        user_ = Settings::getUserAnonymous();
        TextRepository::printMessage(TextRepository::SERVER_LOGGED, connectionId_, Settings::getUserAnonymous().getName().c_str());
        sendMessage(Command(TextRepository::SERVER_COMMAND_OK));
    }
    timeLogon_ = Server::getInstance()->getServerTime();
    timeLastActivity_ = timeLogon_;
}

User SessionServer::getUser() {
    return user_;
}

void SessionServer::sendOnlineList() {
    if (user_.getGrantRead()) {

        Command response;
        Command command;
        list<string> onlineList = Server::getInstance()->getOnlineList();
        response.setName(TextRepository::SERVER_COMMAND_PRINT);
        string fileOnlineStr;
        for (list<string>::iterator it = onlineList.begin(); it != onlineList.end(); it++) {
            response.clearArgs();
            response.addArg(*it);
            sendMessage(response);
            command = recvMessage();
            if (command.getName().compare(TextRepository::CLIENT_COMMAND_OK)) {
                break;
            }
        }
        sendMessage(Command(TextRepository::SERVER_COMMAND_STOP));
    } else {
        Command error(TextRepository::SERVER_COMMAND_STOP);
        error.addArg(TextRepository::CLIENT_NOPRIVILEGES);
        sendMessage(error);
    }
}

string SessionServer::getTimeLogon() {
    return timeLogon_;
}

string SessionServer::getTimeLastActivity() {
    return timeLastActivity_;
}