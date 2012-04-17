#pragma once

#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class TextRepository
{
public:
    static string APP_ARGC;
    static string APP_ERROR_INVALIDPARAM;
    static string APP_ERROR_FILENOTFOUND;
    static string SERVER_STARTED;
    static string SERVER_LISTEN;
    static string SERVER_NEW_CONNECTION;
    static string SERVER_EXECUTE_COMMAND;
    static string SERVER_DISCONNECT;
    static string SERVER_LOGGED;
    static string SERVER_USER_REGISTERED;
    static string SERVER_ERROR_SOCKET;
    static string SERVER_ERROR_BIND;
    static string SERVER_ERROR_LISTEN;
    static string SERVER_ERROR_COMMAND_ARG;
    static string SERVER_ERROR_SEND;
    static string SERVER_ERROR_RECV;
    static string SERVER_ERROR_FILENOTFOUND;
    static string SERVER_ERROR_FILECREATION;
    static string SERVER_ERROR_CREATEDIR;
    static string SERVER_ERROR_LOGIN;
    static string SERVER_ERROR_NOTREGISTERED;
    static string CLIENT_CONNECT;
    static string CLIENT_CONNECT_SUCCESS;
    static string CLIENT_CONSOLE;
    static string CLIENT_CONSOLE_PREFIX;
    static string CLIENT_CONSOLE_PRINT_PATTERN;
    static string CLIENT_DATETIME_PATTERN;
    static string CLIENT_DATETIME_COMPRESSED_PATTERN;
    static string CLIENT_LSDIR_PATTERN;
    static string CLIENT_LS_PATTERN;
    static string CLIENT_WHO_LABELSPATTERN;
    static string CLIENT_WHO_PATTERN;
    static string CLIENT_WHO_LABEL_ID;
    static string CLIENT_WHO_LABEL_USERNAME;
    static string CLIENT_WHO_LABEL_TIMELOGON;
    static string CLIENT_WHO_LABEL_TIMELASTACTIV;

    static string CLIENT_UNKNOWN_COMMAND;
    static string CLIENT_MISSING_ARGUMENT;
    static string CLIENT_FILE_SAVED;
    static string CLIENT_TERMINATED;
    static string CLIENT_FILENOTFOUND;
    static string CLIENT_DIR_NOT_FOUND;
    static string CLIENT_DISCONNECTED;
    static string CLIENT_CREATEDIR;
    static string CLIENT_REMOVEFILE;
    static string CLIENT_LOGIN_PROMPT;
    static string CLIENT_PASSWD_PROMPT;
    static string CLIENT_INVALID_LOGIN;
    static string CLIENT_NOPRIVILEGES;
    static string CLIENT_ERROR_SOCKET;
    static string CLIENT_ERROR_DNS;
    static string CLIENT_ERROR_CONNECT;
    static string CLIENT_ERROR_COMMAND_ARG;
    static string CLIENT_ERROR_SEND;
    static string CLIENT_ERROR_RECV;
    static string CLIENT_ERROR_FILENOTFOUND;

    static string SERVER_COMMAND_STOP;
    static string SERVER_COMMAND_PRINT;
    static string SERVER_COMMAND_OK;

    static string CLIENT_COMMAND_OK;
    static string CLIENT_COMMAND_STOP;
    static string CLIENT_COMMAND_LS;
    static string CLIENT_COMMAND_CD;
    static string CLIENT_COMMAND_GET;
    static string CLIENT_COMMAND_SEND;
    static string CLIENT_COMMAND_EXIT;
    static string CLIENT_COMMAND_RM;
    static string CLIENT_COMMAND_MKDIR;
    static string CLIENT_COMMAND_LOGIN;
    static string CLIENT_COMMAND_WHO;
    static void printMessage(string message, ...);
    static string sprintMessage(string message, ...);
    static string intToString(int num);

    static const char SPACE = ' ';
    static const char SLASH = '/';
    static const char MATCHALL = '*';

    static string UNIT_B;
    static string UNIT_KB;
    static string UNIT_MB;
    static string UNIT_GB;
    static string UNIT_TB;

    static string SERVER_CONFIG_FILE;
    static string SERVER_CONFIG_MAX_CONN;
    static string SERVER_CONFIG_PORT;
    static string SERVER_CONFIG_ANONYMOUS;
    static string SERVER_CONFIG_REPOSITORY;
    static string SERVER_CONFIG_ANONYMOUS_NAME;
    static string SERVER_CONFIG_ANONYMOUS_GRANTS;

    static string CLIENT_CONFIG_FILE;
    static string CLIENT_CONFIG_IP;
    static string CLIENT_CONFIG_PORT;
    static string CLIENT_CONFIG_REPOSITORY;
    static string USERS_CONFIG_FILE;
    static string BACKUP_CONFIG_FILE;
    static string BACKUP_CONFIG_BACKUPDIR;
    static string BACKUP_CONFIG_USERNAME;
    static string BACKUP_CONFIG_PASSWD;
private:
    TextRepository() {};
    ~TextRepository() {};
};