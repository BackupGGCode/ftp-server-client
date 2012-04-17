#pragma once
#include <string>
#include "User.h"
using namespace std;

class Settings
{
public:
    static const int        BUFFER_LENGTH = 255;
    static const int        BUFFER_LENGTH_BIN = 1024;
    static const int        MESSAGE_LENGTH = 255;
    static const int        FILE_BUFFER_LENGTH = 255;
    
    static void             initServer();
    static void             initClient();
	static void             initBackup();
    static unsigned int     getMaxThreads();
    static unsigned int     getServerPort();
    static string           getServerDir();
    static char*            getServerIP();
    static string           getClientDir();
    static bool             getIsAnonymous();
    static User             getUserAnonymous();
    static string           getBackupUserPasswd();
    static string           getBackupUserName();
private:
                            Settings();
                            ~Settings();
    static unsigned int     maxThreads_;
    static unsigned int     serverPort_;
    static string           serverDir_;
    static string           clientDir_;
    static char             serverIP_[20];
    static bool             isAnonymous_;
    static User             userAnonymous_;
	static string			configFile_;
	static string           backupUserName_;
	static string           backupUserPasswd_;
};