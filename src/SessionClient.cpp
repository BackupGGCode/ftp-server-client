#include <iostream>
#include <fstream>

#include <string>
#include "SessionClient.h"
#include "Client.h"
#include "Session.h"
#include "Command.h"
#include "TextRepository.h"
#include "Settings.h"
#include "Server.h"
#include "FtpException.h"

using namespace std;

SessionClient::SessionClient(Client* client) : Session(client->getSocket(), TextRepository::CLIENT_ERROR_RECV, TextRepository::CLIENT_ERROR_SEND) {
    client_ = client;
}

SessionClient::~SessionClient() {
}

void SessionClient::start() {
    char message[Settings::MESSAGE_LENGTH];
    for(;;) {
        TextRepository::printMessage(TextRepository::CLIENT_CONSOLE_PREFIX, Settings::getServerIP(), location_.getLocation().c_str());
        cin.getline(message, sizeof(message), '\n');
        string TextRepositorytr = string(message);
        sendMessage(TextRepositorytr);
        Command com = recvMessage();
        processCommand(com, Command(TextRepositorytr));
    }
}

void SessionClient::startBackup() {
	Command command(TextRepository::CLIENT_COMMAND_MKDIR);
	string backupDir;
	backupDir.append(Settings::getBackupUserName());
	backupDir.append(1, '_');
	backupDir.append(Server::getServerTimeCompressed());
	command.addArg(backupDir);
    sendMessage(command);
    processCommand(recvMessage(), command);
	command = Command(TextRepository::CLIENT_COMMAND_CD);
	command.addArg(backupDir);
    sendMessage(command);
    processCommand(recvMessage(), command);
    list<string> fileList = getFileList(Location(Settings::getClientDir()));
    for(list<string>::iterator it = fileList.begin(); it != fileList.end(); it++) {
	    command = Command(TextRepository::CLIENT_COMMAND_SEND);
	    command.addArg(*it);
        sendMessage(command);
        processCommand(recvMessage(), command);
    }
	command = Command(TextRepository::CLIENT_COMMAND_EXIT);
    sendMessage(command);
    processCommand(recvMessage(), command);
}

list<string> SessionClient::getFileList(Location location, string dir) {
    list<string> fileList;
    WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
    string path;
    path.append(location.getLocation());
    path.append(1, TextRepository::SLASH);
    path.append(dir);
    path.append(1, TextRepository::SLASH);
    path.append(1, TextRepository::MATCHALL);
    cout << path << "\n";
    hFind = FindFirstFileA(path.c_str(), &FindFileData);
    do {
        if (location.getLocation().empty() || !static_cast<string>(FindFileData.cFileName).compare("..") || !static_cast<string>(FindFileData.cFileName).compare(".")) {
            continue;
        }
        if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            string filePath;
            filePath.append(dir);
            filePath.append(1, TextRepository::SLASH);
            filePath.append(FindFileData.cFileName);
            fileList.push_back(filePath);
        } else {

            string pathDir;
            pathDir.append(dir);
            pathDir.append(1, TextRepository::SLASH);
            pathDir.append(FindFileData.cFileName);
            Command command;
	        command = Command(TextRepository::CLIENT_COMMAND_MKDIR);
	        command.addArg(pathDir);
            sendMessage(command);
            processCommand(recvMessage(), command);
            list<string> listRecursive = getFileList(location, pathDir);
            for(list<string>::iterator it = listRecursive.begin(); it != listRecursive.end(); it++) {
                fileList.push_back(*it);
            }
        }
        
    } while (FindNextFileA(hFind, &FindFileData) != 0);

    FindClose(hFind);
    return fileList;
}

void SessionClient::getFile(Command command, Command executedCommand) {
    string fileName = executedCommand.getArgument(0);
    string dest = executedCommand.getArgument(1);
    if (dest.empty()) {
        dest = Settings::getClientDir();
    }
    dest.append("/");
    dest.append(fileName);
    int filesize = atoi(command.getArgument(0).c_str());
    FILE* file;
    fopen_s(&file, dest.c_str(), "wb");
    if (file != NULL) {
        sendMessage(Command(TextRepository::CLIENT_COMMAND_OK));
		while (filesize > 0) {
			if (filesize > Settings::BUFFER_LENGTH_BIN) {
                recvMessageBin(file);
                sendMessage(Command(TextRepository::CLIENT_COMMAND_OK));
			} else {
                recvMessageBin(file);
			}
			filesize -= Settings::BUFFER_LENGTH_BIN;
		}
        fclose(file);
    } else {
        sendMessage(Command(TextRepository::CLIENT_COMMAND_STOP));
        command = recvMessage();
    }
}

void SessionClient::sendFile(Command command, Command executedCommand) {
    try {
        int fileSize = client_->getFileSize(executedCommand.getArgument(0));
        FILE* file = client_->getFile(executedCommand.getArgument(0));
        Command fileSizeCommand(TextRepository::CLIENT_COMMAND_GET);
        fileSizeCommand.addArg(TextRepository::intToString(fileSize));
        sendMessage(fileSizeCommand);
        Command response = recvMessage();
        if(!response.getName().compare(TextRepository::SERVER_COMMAND_OK)) {
			while(fileSize > 0) {
				if(fileSize >= Settings::BUFFER_LENGTH_BIN) {
                    sendMessageBin(file, Settings::BUFFER_LENGTH_BIN);
                    recvMessage();
				} else {
                    sendMessageBin(file, fileSize);
				}
				fileSize -= Settings::BUFFER_LENGTH_BIN;
			}
            fclose(file);
        }
    } catch(FtpException e) {
        e.what();
    }
}

void SessionClient::processCommand(Command command, Command executedCommand) {
    while (command.getName().compare(TextRepository::SERVER_COMMAND_STOP)) {
        if (!command.getName().compare(TextRepository::SERVER_COMMAND_PRINT)) {
            printf(TextRepository::CLIENT_CONSOLE_PRINT_PATTERN.c_str(), command.getArgument(0).c_str()); 
        } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_CD)) {
            location_.goPath(command.getArgument(0));
        } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_GET)) {
            getFile(command, executedCommand);
        } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_SEND)) {
            sendFile(command, executedCommand);
        } else if (!command.getName().compare(TextRepository::CLIENT_COMMAND_EXIT)) {
            throw FtpException(TextRepository::CLIENT_DISCONNECTED);
        }
        sendMessage(Command(TextRepository::CLIENT_COMMAND_OK));
        command = recvMessage();
    }
    printf(TextRepository::CLIENT_CONSOLE_PRINT_PATTERN.c_str(), command.getArgument(0).c_str()); 
}

void SessionClient::login() {
    bool isIncorrect = false;
    if(!recvMessage().getName().compare(TextRepository::CLIENT_COMMAND_LOGIN)) {
        do {
            if (isIncorrect) {
                TextRepository::printMessage(TextRepository::CLIENT_INVALID_LOGIN);
            }
            char userName[20];
            char passwd[20];
            printf(TextRepository::CLIENT_LOGIN_PROMPT.c_str());
            cin.getline(userName, sizeof(userName), '\n');
            printf(TextRepository::CLIENT_PASSWD_PROMPT.c_str());
            cin.getline(passwd, sizeof(passwd), '\n');
            Command response(TextRepository::CLIENT_COMMAND_LOGIN);
            response.addArg(userName);
            response.addArg(passwd);
            sendMessage(response);
        } while(isIncorrect = !recvMessage().getName().compare(TextRepository::SERVER_COMMAND_STOP));
    }

}

void SessionClient::loginBackup() {
    if(!recvMessage().getName().compare(TextRepository::CLIENT_COMMAND_LOGIN)) {
        Command response(TextRepository::CLIENT_COMMAND_LOGIN);
		response.addArg(Settings::getBackupUserName());
		response.addArg(Settings::getBackupUserPasswd());
        sendMessage(response);

		if(!recvMessage().getName().compare(TextRepository::SERVER_COMMAND_STOP)) {
			throw FtpException(TextRepository::CLIENT_DISCONNECTED);
		}
    }
}