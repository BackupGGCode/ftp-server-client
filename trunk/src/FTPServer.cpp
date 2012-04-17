#pragma comment(lib, "WS2_32.lib")
#include <iostream>
#include <string>
#include "Settings.h"
#include "Server.h"
#include "Client.h"
#include "TextRepository.h"
#include "FtpException.h"

using namespace std;

const string SERVER_FLAG = "-server";
const string CLIENT_FLAG = "-client";
const string BACKUP_FLAG = "-backup";
const string REGUSER_FLAG = "-reguser";

int server_main (int argc, char *argv[]) {
    Settings::initServer();
    User::init();
    Server *server = Server::getInstance();
    server->init();
    server->listen();
    delete server;
    return 0;
}

int client_main (int argc, char *argv[]) {
    Settings::initClient();
    Client* client = Client::getInstance();
    client->init();
    client->connect();
    client->console();
    delete client;
    return 0;
}

int backup_main (int argc, char *argv[]) {
    Settings::initBackup();
    Client* client = Client::getInstance();
    client->init();
    client->connect();
    client->backup();
    delete client;
    return 0;
}

int reguser_main (int argc, char *argv[]) {
    if (argc != 4 && argc != 5) {
        throw FtpException(TextRepository::APP_ARGC);
    }
    string userName = argv[2];
    string userPasswd = argv[3];
    if (argc == 4) {
        User newUser(userName, userPasswd);
        newUser.registerUser();
    } else {
        string grants = argv[4];
        User newUser(userName, userPasswd, grants);
        newUser.registerUser();
    }
    return 0;
}

int main (int argc, char *argv[]) {
    try {
        if (argc < 2) {
            throw FtpException(TextRepository::APP_ARGC);
        }
        if (!string(argv[1]).compare(::SERVER_FLAG)) {
            server_main (argc, argv);
        } else if (!string(argv[1]).compare(::CLIENT_FLAG))  {
            client_main (argc, argv);
        } else if (!string(argv[1]).compare(::REGUSER_FLAG))  {
            reguser_main (argc, argv);
        } else if (!string(argv[1]).compare(::BACKUP_FLAG))  {
            backup_main (argc, argv);
        } else {
            throw FtpException(TextRepository::APP_ARGC);
        }
    } catch (FtpException e) {
        e.what();
    }
    return 0;
}