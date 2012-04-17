#include <list>
#include <string>
#include <Windows.h>
#include "User.h"
#include "md5.h"
#include "FtpException.h"
#include "TextRepository.h"
#include "FileParser.h"

using namespace std;
list<User> User::usersRepository_;
unsigned int User::autoIncrement_;
int User::modificationDate_ = 0;
bool User::reloadLock_ = false;

User::~User() {
}

string User::getName() {
    return name_;
}

void User::init() {
    reloadUserRepository();
}

User User::getUserFromRepository(string name, string passwd, int connectionId) {
    for (list<User>::iterator it = usersRepository_.begin(); it != usersRepository_.end(); it++) {
        if (!(*it).name_.compare(name) && !(*it).passwd_.compare(md5(passwd))) {
            return *it;
        }
    }
    throw FtpException(TextRepository::SERVER_ERROR_LOGIN, connectionId);
}

User::User() {
}

void User::reloadUserRepository() {
    WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
    hFind = FindFirstFileA(TextRepository::USERS_CONFIG_FILE.c_str(), &FindFileData);
    int lastWriteTime = FindFileData.ftLastWriteTime.dwLowDateTime;
    if (lastWriteTime == modificationDate_) {
        return;
    }

    FileParser parser(TextRepository::USERS_CONFIG_FILE);
    parser.parseFile();
    list<string> userNames = parser.getParameters();
    list<User> users;

    for (list<string>::iterator it = userNames.begin(); it != userNames.end(); it++) {
        pair<string, string> userData = User::getUserData(parser.getString(*it));
        users.push_back(User(*it, userData.first, userData.second));
    }
    usersRepository_ = users;
    modificationDate_ = lastWriteTime;
}

bool User::userRegistered() {
    for (list<User>::iterator it = usersRepository_.begin(); it != usersRepository_.end(); it++) {
        if (!(*it).name_.compare(name_) ) {
            return true;
        }
    }
    return false;
}

void User::registerUser() {
    reloadUserRepository();
    FILE* file;
    int ret = fopen_s(&file, TextRepository::USERS_CONFIG_FILE.c_str(), "ab");

    if (ret > 0) {
        throw FtpException(TextRepository::APP_ERROR_FILENOTFOUND, TextRepository::USERS_CONFIG_FILE.c_str());
    }

    string passwdEncrypted = md5(passwd_);

    
    if (userRegistered()) {
        TextRepository::printMessage(TextRepository::SERVER_ERROR_NOTREGISTERED, name_.c_str());
    } else {
        string line;
        line.append("\n");
        line.append(name_);
        line.append(1, FileParser::SEPARATOR);
        line.append(passwdEncrypted);
        line.append(1, FileParser::SEPARATOR);
        if(grantRead_) { 
            line.append(1, 'r');
        }
        if(grantWrite_) { 
            line.append(1, 'w');
        }
        line.append("\n");

        fputs(line.c_str(), file);
        fclose(file);
        TextRepository::printMessage(TextRepository::SERVER_USER_REGISTERED, name_.c_str());
    }
}

User::User(string name, string passwd) {
    autoIncrement_++;
    id_ = autoIncrement_;
    name_ = name;
    passwd_ = passwd;
    grantRead_ = true;
    grantWrite_ = false;
}

User::User(string name, string passwd, string grants) {
    autoIncrement_++;
    id_ = autoIncrement_;
    name_ = name;
    passwd_ = passwd;
    if (grants.find('r') != string::npos) {
        grantRead_ = true;
    } else {
        grantRead_ = false;
    }

    if (grants.find('w') != string::npos) {
        grantWrite_ = true;
    } else {
        grantWrite_ = false;
    }
}

void User::waitForUnlock() {
    while(!reloadLock_) {
        Sleep(500);
    }
    reloadLock_ = true;
}

pair<string, string> User::getUserData(string data) {
    string passwd;
    string privileges;
    bool isPasswd = true;
    for(unsigned int i = 0; i < data.length(); i++) {
        char ch = data.at(i);
        if(ch == FileParser::SEPARATOR) {
            isPasswd = false;
        } else {
            if(isPasswd) {
                passwd.append(1, ch);
            } else {
                privileges.append(1, ch);
            }
        }
    }
    return pair<string, string>(passwd, privileges);
}

bool User::getGrantRead() {
    return grantRead_;
}

bool User::getGrantWrite() {
    return grantWrite_;
}