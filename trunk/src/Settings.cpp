#include <string>
#include <iostream>
#include "Settings.h"
#include "FileParser.h"
#include "TextRepository.h"

using namespace std;

unsigned int Settings::maxThreads_;
unsigned int Settings::serverPort_;
string Settings::serverDir_;
char Settings::serverIP_[20];
string Settings::clientDir_;
bool Settings::isAnonymous_;
User Settings::userAnonymous_;
string Settings::configFile_;
string Settings::backupUserName_;
string Settings::backupUserPasswd_;
Settings::Settings() {
}

Settings::~Settings() {
}

void Settings::initServer() {
    FileParser parser(TextRepository::SERVER_CONFIG_FILE);
    parser.parseFile();
    maxThreads_ = parser.getInt(TextRepository::SERVER_CONFIG_MAX_CONN);
    serverPort_ = parser.getInt(TextRepository::SERVER_CONFIG_PORT);
    isAnonymous_ = parser.getBool(TextRepository::SERVER_CONFIG_ANONYMOUS);
    serverDir_ = parser.getString(TextRepository::SERVER_CONFIG_REPOSITORY);
    serverDir_.append(1, TextRepository::SLASH);
    string userAnonymousGrants = parser.getString(TextRepository::SERVER_CONFIG_ANONYMOUS_GRANTS);
    userAnonymous_ = User(TextRepository::SERVER_CONFIG_ANONYMOUS_NAME, "", userAnonymousGrants);
	configFile_ = TextRepository::SERVER_CONFIG_FILE;
}

void Settings::initClient() {
    FileParser parser(TextRepository::CLIENT_CONFIG_FILE);
    parser.parseFile();
    strcpy_s(serverIP_, 20, parser.getString(TextRepository::CLIENT_CONFIG_IP).c_str());
    serverPort_ = parser.getInt(TextRepository::CLIENT_CONFIG_PORT);
    clientDir_ = parser.getString(TextRepository::CLIENT_CONFIG_REPOSITORY);
    clientDir_.append(1, TextRepository::SLASH);
	configFile_ = TextRepository::CLIENT_CONFIG_FILE;
}

void Settings::initBackup() {
    FileParser parser(TextRepository::BACKUP_CONFIG_FILE);
    parser.parseFile();
    strcpy_s(serverIP_, 20, parser.getString(TextRepository::CLIENT_CONFIG_IP).c_str());
    serverPort_ = parser.getInt(TextRepository::CLIENT_CONFIG_PORT);
    clientDir_ = parser.getString(TextRepository::BACKUP_CONFIG_BACKUPDIR);
    clientDir_.append(1, TextRepository::SLASH);
	configFile_ = TextRepository::BACKUP_CONFIG_FILE;
	backupUserName_ = parser.getString(TextRepository::BACKUP_CONFIG_USERNAME);
	backupUserPasswd_ = parser.getString(TextRepository::BACKUP_CONFIG_PASSWD);
}

unsigned int     Settings::getMaxThreads() {
    return maxThreads_;
}


unsigned int     Settings::getServerPort() {
    return serverPort_;
}

string     Settings::getServerDir() {
    return serverDir_;
}

string     Settings::getBackupUserName() {
    return backupUserName_;
}

string     Settings::getBackupUserPasswd() {
    return backupUserPasswd_;
}

char* Settings::getServerIP() {
    return serverIP_;
}

string Settings::getClientDir() {
    return clientDir_;
}

bool Settings::getIsAnonymous() {
    return isAnonymous_;
}

User Settings::getUserAnonymous() {
    return userAnonymous_;
}