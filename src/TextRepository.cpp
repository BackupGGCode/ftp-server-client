#include <cstdarg>
#include <iostream>
#include <string>
#include "TextRepository.h"
#include "Settings.h"

using namespace std;

string TextRepository::APP_ARGC =                 "Invalid commandline parameters\n";
string TextRepository::APP_ERROR_INVALIDPARAM =   "Parameter \"%s\" in file \"%s\" is invalid\n";
string TextRepository::APP_ERROR_FILENOTFOUND =   "File \"%s\" not found or access denied\n";

// Server TextRepository
string TextRepository::SERVER_STARTED =           "[Server] Server started succesfully\n";
string TextRepository::SERVER_LISTEN =            "[Server] Listening on port %d\n";
string TextRepository::SERVER_NEW_CONNECTION =    "[Client][%d] Connected to %s\n";
string TextRepository::SERVER_LOGGED =            "[Client][%d] Logged as \"%s\"\n";
string TextRepository::SERVER_EXECUTE_COMMAND =   "[Client][%d] Executing command: %s\n";
string TextRepository::SERVER_DISCONNECT =        "[Client][%d] Disconnected\n";
string TextRepository::SERVER_USER_REGISTERED =   "User \"%s\" succesfully registered\n";
// Server errors
string TextRepository::SERVER_ERROR_SOCKET =      "[Server] Failed on initializing local socket\n";
string TextRepository::SERVER_ERROR_BIND =        "[Server] Failed on binding the local socket\n";
string TextRepository::SERVER_ERROR_LISTEN =      "[Server] Error while setting listening mode\n";
string TextRepository::SERVER_ERROR_COMMAND_ARG = "[Server] Requested command argument doesnt exist\n";
string TextRepository::SERVER_ERROR_SEND =        "[Client][%d] Disconnected (force)\n";
string TextRepository::SERVER_ERROR_RECV =        "[Client][%d] Disconnected (force)\n";
string TextRepository::SERVER_ERROR_LOGIN =       "[Client][%d] Login failed\n";
string TextRepository::SERVER_ERROR_FILENOTFOUND ="[Server] File \"%s\" not found or access denied\n";
string TextRepository::SERVER_ERROR_FILECREATION ="[Server] Cannot create file \"%s\"\n";
string TextRepository::SERVER_ERROR_CREATEDIR =   "[Server] Cannot create directory \"%s\"\n";
string TextRepository::SERVER_ERROR_NOTREGISTERED = "User \"%s\" cannot be registered\n";
// Client TextRepository
string TextRepository::CLIENT_CONNECT =           "Connecting to %s:%d ...\n";
string TextRepository::CLIENT_CONNECT_SUCCESS =   "Connected\n";
string TextRepository::CLIENT_CONSOLE =           "Console ready\n";
string TextRepository::CLIENT_CONSOLE_PREFIX =    "\n%s/%s> ";
string TextRepository::CLIENT_CONSOLE_PRINT_PATTERN = "%s\n";
string TextRepository::CLIENT_LSDIR_PATTERN =      "%5c %s";
string TextRepository::CLIENT_LS_PATTERN =         "%3d%-2s %s";
string TextRepository::CLIENT_DATETIME_PATTERN =   "%02d:%02d:%02d %4d-%02d-%02d";
string TextRepository::CLIENT_DATETIME_COMPRESSED_PATTERN = "%4d%02d%02d_%02d%02d%02d";

string TextRepository::CLIENT_WHO_LABELSPATTERN =  "%6s %15s %25s %25s";
string TextRepository::CLIENT_WHO_PATTERN =        "%6d %15s %25s %25s";
string TextRepository::CLIENT_WHO_LABEL_ID =       "-ID-";
string TextRepository::CLIENT_WHO_LABEL_USERNAME = "-User name-";
string TextRepository::CLIENT_WHO_LABEL_TIMELOGON ="-Logon time-";
string TextRepository::CLIENT_WHO_LABEL_TIMELASTACTIV ="-Last activity-";

string TextRepository::CLIENT_UNKNOWN_COMMAND =   "Unknown command\n";
string TextRepository::CLIENT_MISSING_ARGUMENT =  "missing argument list";
string TextRepository::CLIENT_FILE_SAVED =        "file saved";
string TextRepository::CLIENT_TERMINATED =        "terminated";
string TextRepository::CLIENT_FILENOTFOUND =      "file doesnt exist";
string TextRepository::CLIENT_DIR_NOT_FOUND =     "directory not found";
string TextRepository::CLIENT_DISCONNECTED =      "disconnected";
string TextRepository::CLIENT_NOPRIVILEGES =      "insufficient privileges";
string TextRepository::CLIENT_CREATEDIR =         "cannot create directory";
string TextRepository::CLIENT_REMOVEFILE =        "cannot remove file";
string TextRepository::CLIENT_LOGIN_PROMPT =      "User name:";
string TextRepository::CLIENT_PASSWD_PROMPT =     "Password:";
string TextRepository::CLIENT_INVALID_LOGIN =     "Invalid username and/or password\n";

// Client errors
string TextRepository::CLIENT_ERROR_SOCKET =      "Failed on initializing local socket\n";
string TextRepository::CLIENT_ERROR_DNS =         "Cannot get ip from host\n";
string TextRepository::CLIENT_ERROR_CONNECT =     "Cannot connect to the server\n";
string TextRepository::CLIENT_ERROR_COMMAND_ARG = "Requested command argument doesnt exist\n";
string TextRepository::CLIENT_ERROR_SEND =        "Failed while sending message to the remote server\n";
string TextRepository::CLIENT_ERROR_FILENOTFOUND ="File not found or access denied\n";
string TextRepository::CLIENT_ERROR_RECV =        "Failed while receiving message from the remote server\n";

// Server commands
string TextRepository::SERVER_COMMAND_STOP =      "stop";
string TextRepository::SERVER_COMMAND_PRINT =     "print";
string TextRepository::SERVER_COMMAND_OK  =       "ok";

// Client commands
string TextRepository::CLIENT_COMMAND_OK  =       "ok";
string TextRepository::CLIENT_COMMAND_STOP  =     "stop";
string TextRepository::CLIENT_COMMAND_LS  =       "ls";
string TextRepository::CLIENT_COMMAND_CD =        "cd";
string TextRepository::CLIENT_COMMAND_GET =       "get";
string TextRepository::CLIENT_COMMAND_SEND =      "send";
string TextRepository::CLIENT_COMMAND_EXIT =      "exit";
string TextRepository::CLIENT_COMMAND_RM =        "rm";
string TextRepository::CLIENT_COMMAND_LOGIN =     "login";
string TextRepository::CLIENT_COMMAND_MKDIR =     "mkdir";
string TextRepository::CLIENT_COMMAND_WHO =       "who";

string TextRepository::UNIT_B =                   "B";
string TextRepository::UNIT_KB =                  "kB";
string TextRepository::UNIT_MB =                  "MB";
string TextRepository::UNIT_GB =                  "GB";
string TextRepository::UNIT_TB =                  "TB";

string TextRepository::SERVER_CONFIG_FILE =       "server.ini";
string TextRepository::SERVER_CONFIG_MAX_CONN =   "MaxConnections";
string TextRepository::SERVER_CONFIG_PORT =       "ServerPort";
string TextRepository::SERVER_CONFIG_ANONYMOUS =  "AnonymousAccess";
string TextRepository::SERVER_CONFIG_REPOSITORY = "FileRepositoryPath";
string TextRepository::SERVER_CONFIG_ANONYMOUS_NAME = "Anonymous";
string TextRepository::SERVER_CONFIG_ANONYMOUS_GRANTS = "AnonymousGrants";


string TextRepository::CLIENT_CONFIG_FILE =       "client.ini";
string TextRepository::CLIENT_CONFIG_IP =         "ServerIP";
string TextRepository::CLIENT_CONFIG_PORT =       "ServerPort";
string TextRepository::CLIENT_CONFIG_REPOSITORY = "FileRepositoryPath";

string TextRepository::BACKUP_CONFIG_FILE =       "backup.ini";
string TextRepository::BACKUP_CONFIG_BACKUPDIR =  "BackupDir";
string TextRepository::BACKUP_CONFIG_USERNAME =   "UserName";
string TextRepository::BACKUP_CONFIG_PASSWD =     "UserPasswd";

string TextRepository::USERS_CONFIG_FILE =        "users.ini";

void TextRepository::printMessage(string message, ...) {
    va_list args;
    va_start(args, message);
    vprintf(message.c_str(), args);
    va_end(args);
}

string TextRepository::sprintMessage(string message, ...) {
    va_list args;
    va_start(args, message);
    char buffer[Settings::MESSAGE_LENGTH];
    vsprintf_s(buffer, message.c_str(), args);
    va_end(args);
    return string(buffer);
}

string TextRepository::intToString(int num) {
    ostringstream ss;
    ss << num;
    return ss.str();
}