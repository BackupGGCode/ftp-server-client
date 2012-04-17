#pragma once
#include <list>
#include <string>

using namespace std;
class User
{
public:
    User();
    ~User();
    User(string name, string passwd);
    User(string name, string passwd, string grants);
    string getName();
    bool getGrantRead();
    bool getGrantWrite();
    void registerUser();
    static void init();
    static User getUserFromRepository(string name, string passwd, int connectionId);
    static void reloadUserRepository();
    bool userRegistered();
private:
    string name_;
    string passwd_;
    int id_;
    bool grantRead_;
    bool grantWrite_;
    static list<User> usersRepository_;
    static unsigned int autoIncrement_;
    static int modificationDate_;
    static bool reloadLock_;
    static void waitForUnlock();
    static pair<string, string> getUserData(string data);
};