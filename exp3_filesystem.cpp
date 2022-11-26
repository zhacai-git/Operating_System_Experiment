#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cstring>
#include <ctime>
#include <sstream>
#include <windows.h>

using namespace std;


// 用户类全局变量
bool hasUser;
int uidCounter = 1; // 0 is a system reserved uid
int userPointer = 0;
int fileCounter = 1;

struct user {
    string username;
    bool userLoggedIn;
    int userP;
    string userPwd;
    int uid;
};

struct time_f { // 时间结构
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

struct FCB { // 文件控制结构
    int file_type; // 1-normal file 2-directory
    string file_name;
    int f_id;
    FCB *root_dir; // parent directory
    int root_id;
    int f_uid;
    int user_p_flag[3]; // position: 0-read 1-write 2-executable
    int extern_p_flag[3]; // non-permission-user flag: 0-read 1-write 2-executable
    time_f create_time;
    time_f modify_time;
    int f_flag; // 1-open 2-closed 3-marked-delete(rare)
    int d_block; // file location on the disk vector
    vector<FCB> sub_files; // use when file type is directory
};

FCB *userPwd; // current user pwd pointer

struct disk {
    int block_stat; // 0-idle 1-used
    string block_data;
};

// 全局变量定义
vector<user> userList;
vector<FCB> fileSys;
vector<disk> diskBlock;



// 函数定义区

int addUser();
int deleteUser(string username);
int listUser();

int setTime(FCB *modify);
int setTime(vector<FCB>::iterator modify);
int setModifyTime(vector<FCB>::iterator file);
string getTime(time_f time);

int dir_list(string args);
int mkdir(string dir_name);
int touch(string filename);
int cd(string args);
int rm(string args1, string args2);
int su(string username);
int chmod_pre(const string& filename, int args);
int chmod(vector<FCB>::iterator file, int args);
int chown(string filename, string username);
int cat(string filename);
int echo(string filename, string content);
int listDisk();
int stat(const string& filename);
int help();

int fileOpen(vector<FCB>::iterator file, int read, int write, int execute);
bool filePermissionCheck(vector<FCB>::iterator file);
bool fileConflictDetect(const string& filename);
bool dirConflictDetect(const string& dir_name);
int diskBlockAllocation(string content);
string getFileOwnerUsername(FCB *file);
string getUsernameByUid(int uid);
int getUidByUsername(string username);
int generateDemonstrate();

vector<string> split(string str, char split);

constexpr unsigned int str2int(const char* str,int h = 0) { // string转unicode, 控制台switch匹配
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

int commandLineHandler();
int systemInitialization();


int main() {
//    commandLineHandler("", "",0);
    systemInitialization();
    if (!hasUser) {
        addUser();
        userPwd = &fileSys[0];
        commandLineHandler();
    }
}

int commandLineHandler() {
    cin.ignore();
    while (true) {
        cout << userList[userPointer].username << "@file_system_demo:" << userList[userPointer].userPwd << (userList[userPointer].userP == 1 ? "# " : "$ ");
        string cmd;
        getline(cin, cmd);
        if (cmd.empty()) {
            continue;
        }
//        cout << cmd << endl;
        vector<string> cmd_parse;
        cmd_parse = split(cmd, ' ');
//        cout << cmd_parse.size() << endl;
        switch (str2int(cmd_parse[0].c_str())) {
            case str2int("ls"):
                if (cmd_parse.size() == 2) {
                    dir_list(cmd_parse[1]);
                } else {
                    dir_list("");
                }
                break;
            case str2int("mkdir"):
                if (cmd_parse.size() == 2) {
                    mkdir(cmd_parse[1]);
                } else {
                    mkdir("");
                }
                break;
            case str2int("touch"):
                if (cmd_parse.size() == 2) {
                    touch(cmd_parse[1]);
                } else {
                    touch("");
                }
                break;
            case str2int("cd"):
                if (cmd_parse.size() == 2) {
                    cd(cmd_parse[1]);
                } else {
                    cd("");
                }
                break;
            case str2int("rm"):
                if (cmd_parse.size() == 2) {
                    rm(cmd_parse[1],"");
                } else if (cmd_parse.size() == 3) {
                    rm(cmd_parse[1],cmd_parse[2]);
                } else {
                    rm("","");
                }
                break;
            case str2int("useradd"):
                addUser();
                break;
            case str2int("lsuser"):
                listUser();
                break;
            case str2int("lsdisk"):
                listDisk();
                break;
            case str2int("su"):
                if (cmd_parse.size() == 2) {
                    su(cmd_parse[1]);
                } else {
                    su("");
                }
                break;
            case str2int("chmod"):
                if (cmd_parse.size() == 3) {
                    chmod_pre(cmd_parse[1], atoi(cmd_parse[2].c_str()));
                } else {
                    cout << "chmod: missing arguments" << endl;
                }
                break;
            case str2int("chown"):
                if (cmd_parse.size() == 3) {
                    chown(cmd_parse[1],cmd_parse[2]);
                } else {
                    chown("","");
                }
                break;
            case str2int("cat"):
                if (cmd_parse.size() == 2) {
                    cat(cmd_parse[1]);
                } else {
                    cat("");
                }
                break;
            case str2int("echo"):
                if (cmd_parse.size() == 3) {
                    echo(cmd_parse[1], cmd_parse[2]);
                } else {
                    echo("","");
                }
                break;
            case str2int("stat"):
                if (cmd_parse.size() == 2) {
                    stat(cmd_parse[1]);
                } else {
                    stat("");
                }
                break;
            case str2int("clear"):
                system("cls");
                break;
            case str2int("help"):
                help();
                break;
            case str2int("demo"):
                generateDemonstrate();
                break;
            case str2int("exit"):
                exit(0);
            default:
                cout << cmd_parse[0] << ": command not found" << endl;
                continue;
        }
    }
}

int addUser() {
    user user_add;

    cout << ":# Please enter username: ";
    cin >> user_add.username;;
    if (hasUser) {
        cout << ":# Please enter user permission(0-normal user / 1-administrator): ";
        cin >> user_add.userP;
        user_add.userLoggedIn = false;
    } else {
        cout << "No user detected, permission has set to administrator by default!" << endl;
        user_add.userP = 1;
        user_add.userLoggedIn = true;
    }
    user_add.userPwd = "/";
    user_add.uid = uidCounter;
    userList.push_back(user_add);
    cout << ":# Success, new user: " << user_add.username << (user_add.userP == 0 ? " (normal user)" : " (administrator)") << " add." << endl;
    if (hasUser) cin.ignore();
    uidCounter++;
    hasUser = true;
    return hasUser ? 0 : 1;
}

vector<string> split(string str, char split) {
    vector<string> items;
    istringstream iss(str);	// 输入流
    string token;			// 接收缓冲区
    while (getline(iss, token, split))	// 以split为分隔符
    {
        items.push_back(token);
    }
    return items;
}

int systemInitialization() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    FCB rootFile;
    rootFile.file_name = "/";

    setTime(&rootFile);

    rootFile.file_type = 2;
    rootFile.d_block = 0;
    rootFile.f_uid = 0;
    rootFile.f_id = 0;

    rootFile.modify_time = rootFile.create_time;

    rootFile.user_p_flag[0] = 1; rootFile.user_p_flag[1] = 1; rootFile.user_p_flag[2] = 0;

    rootFile.extern_p_flag[0] = 0; rootFile.extern_p_flag[1] = 0; rootFile.extern_p_flag[2] = 0;

    rootFile.root_dir = nullptr;
    rootFile.root_id = -1;
    rootFile.f_flag = 1;
    disk t_disk;
    t_disk.block_stat = 1;
    t_disk.block_data = "system root directory";

    diskBlock.push_back(t_disk);

    FCB rootFile_sub;

    rootFile_sub.file_type = 1;
    rootFile_sub.file_name = "testfile.demo";
    rootFile_sub.user_p_flag[0] = 1; rootFile_sub.user_p_flag[1] = 1; rootFile_sub.user_p_flag[2] = 1;
    rootFile_sub.extern_p_flag[0] = 1; rootFile_sub.extern_p_flag[1] = 0; rootFile_sub.extern_p_flag[2] = 1;
    rootFile_sub.root_dir = &rootFile;
    rootFile_sub.root_id = 0;
    rootFile_sub.f_id = 1;
    rootFile_sub.f_flag = 0;
    rootFile_sub.f_uid = 0;
    setTime(&rootFile_sub);
    rootFile_sub.modify_time = rootFile.create_time;

    rootFile_sub.d_block = diskBlockAllocation("system created test file.");

    rootFile.sub_files.push_back(rootFile_sub);

    fileSys.push_back(rootFile);
    fileCounter++;
//    mkdir("home"); // deprecated: causing array out of bound error
    return 0;
}

int setTime(FCB *modify) {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    modify->create_time.year = ltm->tm_year + 1900;
    modify->create_time.month = ltm->tm_mon + 1;
    modify->create_time.day = ltm->tm_mday;
    modify->create_time.hour = ltm->tm_hour;
    modify->create_time.minute = ltm->tm_min;
    modify->create_time.second = ltm->tm_sec;

    return 0;
}

int setTime(vector<FCB>::iterator modify) {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    modify->create_time.year = ltm->tm_year + 1900;
    modify->create_time.month = ltm->tm_mon + 1;
    modify->create_time.day = ltm->tm_mday;
    modify->create_time.hour = ltm->tm_hour;
    modify->create_time.minute = ltm->tm_min;
    modify->create_time.second = ltm->tm_sec;

    return 0;
}

int setModifyTime(vector<FCB>::iterator file) {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    file->modify_time.year = ltm->tm_year + 1900;
    file->modify_time.month = ltm->tm_mon + 1;
    file->modify_time.day = ltm->tm_mday;
    file->modify_time.hour = ltm->tm_hour;
    file->modify_time.minute = ltm->tm_min;
    file->modify_time.second = ltm->tm_sec;

    return 0;
}

int dir_list(string args) {
//    string userPwd = userList[userPointer].userPwd;
    if (args.empty()) {
        if (userPwd->sub_files.size() == 0) {
            return 0;
        } else {
            for (auto & sub_file : userPwd->sub_files) {
                cout << sub_file.file_name << " ";
            }
            cout << endl;
        }
        return 0;
    }
    switch (str2int(args.c_str())) {
        case str2int("--help"):
            cout << "List information about the FILEs (the current directory by default)" << endl;
            cout << "Usage: ls [OPTION]" << endl;
            cout << "-l  use a long listing format(detailed)" << endl;
            break;
        case str2int("-l"):
            for (auto & sub_file : userPwd->sub_files) {
                cout << (sub_file.file_type == 2 ? "d" : "-") << (sub_file.user_p_flag[0] ? "r" : "-") << (sub_file.user_p_flag[1] ? "w" : "-") << (sub_file.user_p_flag[2] ? "x" : "-");
                cout << (sub_file.extern_p_flag[0] ? "r" : "-") << (sub_file.extern_p_flag[1] ? "w" : "-") << (sub_file.extern_p_flag[2] ? "x" : "-");
                cout << " " << getFileOwnerUsername(&sub_file);
                cout << " " << sub_file.modify_time.year << " " << sub_file.modify_time.month << " " << sub_file.modify_time.day << " ";
                cout << sub_file.modify_time.hour << " " << sub_file.modify_time.minute << " ";
                cout << sub_file.file_name << " "  << endl;
            }
            break;
    }
}

string getFileOwnerUsername(FCB *file) {
    int f_uid = file->f_uid;
    if (f_uid == 0) {
        return "system";
    }
    for (auto & it : userList) {
        if (f_uid == it.uid) {
            return it.username;
        }
    }
}

int mkdir(string dir_name) {
    FCB t_dir;
    if (dir_name.empty()) {
        cout << "mkdir: missing operand" << endl;
        return 1;
    }
    if (dirConflictDetect(dir_name)) {
        cout << "mkdir: " << dir_name << ": Already exist" << endl;
        return 0;
    }
    t_dir.file_name = dir_name;

    setTime(&t_dir);

    t_dir.file_type = 2;
    t_dir.d_block = diskBlockAllocation("directory file");
    t_dir.f_uid = userList[userPointer].uid;
    t_dir.f_id = fileCounter;

    t_dir.modify_time = t_dir.create_time;

    t_dir.user_p_flag[0] = 1; t_dir.user_p_flag[1] = 0; t_dir.user_p_flag[2] = 0;

    t_dir.extern_p_flag[0] = 0; t_dir.extern_p_flag[1] = 0; t_dir.extern_p_flag[2] = 0;

    t_dir.root_dir = userPwd;
    t_dir.root_id = userPwd->f_id;
    t_dir.f_flag = 0;
    userPwd->sub_files.push_back(t_dir);

    return 0;
}

bool dirConflictDetect(const string& dir_name) {
    for (auto & sub_file : userPwd->sub_files) {
        if (sub_file.file_name == dir_name && sub_file.file_type == 2) {
            return true;
        }
    }
    return false;
}

int touch(string filename) {
    FCB t_file;
    if (filename.empty()) {
        cout << "touch: missing file operand" << endl;
        return 1;
    }
    t_file.file_name = filename;

    if (fileConflictDetect(filename)) {
        return 0;
    }

    setTime(&t_file);

    t_file.file_type = 1;
    t_file.d_block = diskBlockAllocation("");
    t_file.f_uid = userList[userPointer].uid;
    t_file.f_id = fileCounter;

    t_file.modify_time = t_file.create_time;

    t_file.user_p_flag[0] = 1; t_file.user_p_flag[1] = 1; t_file.user_p_flag[2] = 0;

    t_file.extern_p_flag[0] = 1; t_file.extern_p_flag[1] = 0; t_file.extern_p_flag[2] = 0;

    t_file.root_dir = userPwd;
    t_file.root_id = userPwd->f_id;
    t_file.f_flag = 0;
    userPwd->sub_files.push_back(t_file);
    return 0;
}

bool fileConflictDetect(const string& filename) {
    for (auto sub_file = userPwd->sub_files.begin(); sub_file != userPwd->sub_files.end(); sub_file++) {
//        if (sub_file->file_name == filename && sub_file->f_uid == userList[userPointer].uid) {
        if (sub_file->file_name == filename) {
            if (sub_file->file_type == 2) {
                cout << "touch: " << filename << ": Is a directory" << endl;
                return true;
            }
            if (filePermissionCheck(sub_file)) {
                setModifyTime(sub_file);
                return true;
            } else {
                cout << "touch: permission denied" << endl;
                return true;
            }
        }
    }
    return false;
}

int diskBlockAllocation(string content) {
    int diskBlockCounter = 0;
    for (auto & disk_it : diskBlock) {
        if (disk_it.block_stat == 0) {
            disk_it.block_data = content;
            disk_it.block_stat = 1;
            return diskBlockCounter;
        }
        diskBlockCounter++;
    }
    disk t_disk;
    t_disk.block_stat = 1;
    t_disk.block_data = content;
    diskBlock.push_back(t_disk);
    return diskBlock.size() - 1;
}

int fileTypeCheck(string filename) {
    for (auto & file_t : userPwd->sub_files) {
        if (file_t.file_name == filename) {
            return file_t.file_type;
        }
    }
    return 0;
}

int cd(string args) {
    string userPwd_s = userList[userPointer].userPwd;
    vector<string> userPwd_p = split(userPwd_s, '/');
    if (args.empty()) {
        return 0;
    }
    switch (str2int(args.c_str())) {
        case str2int(".."):
            if (userPwd->root_dir != nullptr) { // 非根目录
                userPwd_p.pop_back(); // 弹出
                userPwd = userPwd->root_dir; // userPwd指向上一个文件夹
            } else {
                return 0;
            }
            break;
        default:
            switch (fileTypeCheck(args)) {
                case 0:
                    cout << "cd: " << args << ": No such file or directory" << endl;
                    return 1;
                case 1:
                    cout << "cd: " << args << ": Not a directory" << endl;
                    return 1;
                case 2:
                    for (auto & file_t : userPwd->sub_files) {
                        if (file_t.file_name == args && file_t.file_type == 2) {
                            userPwd = &file_t;
                            userPwd_p.push_back(file_t.file_name);
                        }
                    }
            }
    }
    userList[userPointer].userPwd = "";
    for (int i = 0; i < userPwd_p.size();i++) {
        if (userPwd_p[i] != "") {
            userList[userPointer].userPwd += "/";
            userList[userPointer].userPwd += userPwd_p[i];
        }
    }
    if (userPwd_p.size() == 1 && userPwd_p[0].empty()) userList[userPointer].userPwd = "/";
    return 0;
}

int rm_R(vector<FCB>::iterator dir);

int rm(string args1, string args2) {
    if (args1.empty()) {
        cout << "rm: missing operand" << endl;
        return 1;
    } else if (args2.empty()) { // args1 exist & args2 not exist, means delete a normal file not directory, filename uses args1
        for (auto it = userPwd->sub_files.begin(); it != userPwd->sub_files.end(); it++) {
            if (it->file_name == args1) {
                if (it->file_type == 1 && (it->f_uid == userList[userPointer].uid || userList[userPointer].userP == 1) && it->f_uid != 0) { // run execution
//                    cout << "file to be deleted: " << it->file_type << " " << it->file_name << " " << it->f_uid << " current user: " << userList[userPointer].uid << " " << userList[userPointer].userP << endl;
                    diskBlock[it->d_block].block_stat = 0; // stat set to 0, but not erase the data on it
                    userPwd->sub_files.erase(it);
                    return 0;
                } else {
                    if (it->f_uid != userList[userPointer].uid || userList[userPointer].userP != 1 || it->f_uid == 0) {
                        cout << "rm: cannot delete '" << args1 << "': permission denied" << endl;
                        return 0;
                    }
                    if (it->file_type == 2) {
                        cout << "rm: cannot remove '" << it->file_name << "' : Is a directory" << endl;
                        cout << "rm: hint: use -r to delete a directory" << endl;
                        return 0;
                    }

                    return 1;
                }
            }
        }
        cout << "rm: cannot remove '" << args1 << "': No such file or directory" << endl;
        return 0;
    }
    if (args1 == "-r") { //  -r, -R, --recursive   remove directories and their contents recursively
        if (args2.empty()) {
            cout << "rm: missing operand" << endl;
        } else {
            for (auto it = userPwd->sub_files.begin(); it != userPwd->sub_files.end(); it++) {
//                cout << "Reached file " << it->file_name << endl;
                if (it->file_name == args2) { // 这个时候的it迭代器指向要删除的目录
                    if (it->file_type == 2) {
                        if (filePermissionCheck(it)) {
                            rm_R(it);
                            userPwd->sub_files.erase(it);
                            diskBlock[it->d_block].block_stat = 0;
                            return 0;
                        } else {
                            cout << "rm: cannot delete '" << args2 << "': permission denied" << endl;
                            return 1;
                        }
                    } else if (it->file_type == 1) {
                        if (filePermissionCheck(it)) {
                            diskBlock[it->d_block].block_stat = 0; // stat set to 0, but not erase the data on it
                            userPwd->sub_files.erase(it);
                            return 0;
                        } else {
                            cout << "rm: cannot delete '" << args2 << "': permission denied" << endl;
                            return 1;
                        }
                    }
//                    cout << "debug: it address " << &it << endl;
//                    cout << "debug: &(*it) address " <<  << endl;

                    return 0;
                }
            }
            cout << "rm: cannot remove '" << args2 << "': No such file or directory" << endl;
        }
    }
    return 0;
}

int rm_R(vector<FCB>::iterator dir) {
//    cout << "recursive function dir address " << &dir << endl;
    for (auto it = dir->sub_files.begin(); it != dir->sub_files.end();) {
        switch (it->file_type) {
            case 1:
//                cout << "deleting file " << it->file_name << endl;
                diskBlock[it->d_block].block_stat = 0; // stat set to 0, but not erase the data on it
                it = dir->sub_files.erase(it);
                break;
            case 2:
                rm_R(it);
//                cout << "deleting directory " << it->file_name << endl;
                diskBlock[it->d_block].block_stat = 0;
                it = dir->sub_files.erase(it);
                break;
        }
    }
    return 0;
}

int su(string username) {
    for (int i = 0; i < userList.size(); i++) {
        if (userList[i].username == username) {
            userList[userPointer].userLoggedIn = false;
            userPointer = i;
            userList[userPointer].userLoggedIn = true;
            while (userPwd->root_dir != nullptr) {
                userPwd = userPwd->root_dir;
            }
            cout << "user changed" << endl;
            return 0;
        }
    }
    cout << "su: user not found" << endl;
    return 1;
}

int chmod_pre(const string& filename, int args) {
    for (auto it = userPwd->sub_files.begin(); it != userPwd->sub_files.end(); it++) {
        if (it->file_name == filename ) {
            if (filePermissionCheck(it)) {// user self-manage or administrator permission override, system reserved file are not allowed to change permission
                if (it->file_type == 2) {
                    cout << "chmod: " << it->file_name << ": Is a directory" << endl;
                    break;
                } else {
                    chmod(it, args);
                    break;
                }
            } else {
                cout << "chmod: permission denied" << endl;
            }
        }
    }
    return 0;
}

int chmod(vector<FCB>::iterator file, int args) {
    int user_permission = args / 10;
    int external_permission = args % 10;
    if (user_permission <= 0 || external_permission <= 0) {
        cout << "chmod: invalid permission code" << endl;
    }
//    file->user_p_flag[0] = 1; file->user_p_flag[1] = 0; file->user_p_flag[2] = 0; // user permission
    switch (user_permission) {
        case 4:
            file->user_p_flag[0] = 1; file->user_p_flag[1] = 0; file->user_p_flag[2] = 0; // user permission
            break;
        case 5: // r / x
            file->user_p_flag[0] = 1; file->user_p_flag[1] = 0; file->user_p_flag[2] = 1; // user permission
            break;
        case 6: // r / w
            file->user_p_flag[0] = 1; file->user_p_flag[1] = 1; file->user_p_flag[2] = 0; // user permission
            break;
        case 7: // r / w / x
            file->user_p_flag[0] = 1; file->user_p_flag[1] = 1; file->user_p_flag[2] = 1; // user permission
            break;
        default:
            cout << "chmod: unsupported user permission format" << endl;
            return 1;
    }
    switch (external_permission) {
        case 4:
            file->extern_p_flag[0] = 1; file->extern_p_flag[1] = 0; file->extern_p_flag[2] = 0; // external permission
            break;
        case 5: // r / x
            file->extern_p_flag[0] = 1; file->extern_p_flag[1] = 0; file->extern_p_flag[2] = 1; // external permission
            break;
        case 6: // r / w
            file->extern_p_flag[0] = 1; file->extern_p_flag[1] = 1; file->extern_p_flag[2] = 0; // external permission
            break;
        case 7: // r / w / x
            file->extern_p_flag[0] = 1; file->extern_p_flag[1] = 1; file->extern_p_flag[2] = 1; // external permission
            break;
        default:
            cout << "chmod: unsupported external permission format" << endl;
            return 1;
    }
    return 0;
}

int chown(string filename, string username) {
    if (filename.empty() || username.empty()) {
        cout << "chown: mission operand" << endl;
        return 0;
    }
    for (auto it = userPwd->sub_files.begin(); it != userPwd->sub_files.end(); it++) {
        if (it->file_name == filename) {
            if (userList[userPointer].userP == 0 || it->f_uid == 0) { // 普通用户无权改文件所有权，且系统保留文件不可更改所有权
                cout << "chown: permission denied" << endl;
                return 1;
            } else {
                int t_uid = getUidByUsername(username);
                if (t_uid == 0) {
                    cout << "chown: user not found" << endl;
                    return 0;
                }
                it->f_uid = t_uid;
                setModifyTime(it);
                return 0;
            }
        }
    }
    cout << "chown: " << filename << ": No such file or directory" << endl;
    return 1;
}

int getUidByUsername(string username) {
    for (auto & it : userList) {
        if (it.username == username) {
            return it.uid;
        }
    }
    return 0;
}

bool filePermissionCheck(vector<FCB>::iterator file) {
//    cout << file->f_uid << " " << userList[userPointer].uid << " " << userList[userPointer].userP << endl;
    return (file->f_uid == userList[userPointer].uid || userList[userPointer].userP == 1) && file->f_uid != 0; // if not system reserved file, matches current user or is administrator operating, return true, else false.
}

int cat(string filename) {
    if (filename.empty()) {
        cout << "cat: missing operand" << endl;
        return 0;
    }
    for (auto it = userPwd->sub_files.begin(); it != userPwd->sub_files.end(); it++) {
        if (it->file_name == filename) {
            if ((it->f_uid == userList[userPointer].uid || it->extern_p_flag[0] == 1) && it->file_type == 1) {
                it->f_flag = 1;
//                cout << "file " << filename << " content: " << endl;
//                cout << "d_block: " << it->d_block << endl;
//                cout << "d_block stat " << diskBlock[it->d_block].block_stat << endl;
                if (diskBlock[it->d_block].block_data.empty()) {
                    cout << "cat: file is empty" << endl;
                } else {
                    cout << diskBlock[it->d_block].block_data << endl;
                }
                it->f_flag = 2;
                return 0;
            } else {
                if (it->file_type == 2) {
                    cout << "cat: " << it->file_name << ": Is a directory" << endl;
                    return 0;
                }
                if (it->f_uid != userList[userPointer].uid || it->extern_p_flag[0] != 1 || it->f_uid == 0) {
                    cout << "cat: permission denied" << endl;
                    return 0;
                }
            }
        }
    }
    cout << "cat: " << filename << ": No such file or directory" << endl;
    return 0;
}

int echo(string filename, string content) {
    if (filename.empty() || content.empty()) {
        cout << "echo: missing operand" << endl;
        return 0;
    }
    for (auto it = userPwd->sub_files.begin(); it != userPwd->sub_files.end(); it++) {
        if (it->file_name == filename) {
            if (((filePermissionCheck(it) && it->user_p_flag[1] == 1) || it->extern_p_flag[1] == 1) && it->file_type == 1) {
                it->f_flag = 1;
                diskBlock[it->d_block].block_data += content;
                setModifyTime(it);
                it->f_flag = 2;
                return 0;
            } else {
                if ((!filePermissionCheck(it) || it->user_p_flag[1] != 1)) {
                    cout << "echo: permission denied" << endl;
                    return 0;
                }
                if (it->file_type == 2) {
                    cout << "echo: " << it->file_name << ": Is a directory" << endl;
                    return 0;
                }
            }
        }
    }
    cout << "echo: " << filename << ": No such file or directory" << endl;
    return 0;
}

int listUser() {
    cout << "Total " << userList.size() << " user(s)" << endl;
//    cout << "UID USERNAME PERM" << endl;
    for (auto & it : userList) {
        cout << it.uid << " " << it.username << " " << (it.userP == 1 ? "administrator" : "user\t") << " " << (it.userLoggedIn ? " \t<-current" : "") << endl;
    }
    return 0;
}

int listDisk() {
    cout << "Total " << diskBlock.size() << " block(s) allocated" << endl;
//    cout << "B.NO STAT CONTENT" << endl;
    int diskBlockCounter= 0 ;
    for (auto & it : diskBlock) {
        cout << diskBlockCounter << " " << (it.block_stat == 0 ? "idle" : "used") << " | " << (it.block_stat == 0 ? "*(detached)" : "") <<(userList[userPointer].userP == 1 ? it.block_data : "*permission denied") << endl;
        diskBlockCounter++;
    }
    return 0;
}

string getUsernameByUid(int uid) {
    for (auto & it : userList) {
        if (it.uid == uid) {
            return it.username;
        }
    }
    return "system";
}

int stat(const string& filename) {
    if (filename.empty()) {
        cout << "stat: missing operand" << endl;
        return 0;
    }
    for (auto it = userPwd->sub_files.begin(); it != userPwd->sub_files.end(); it++) {
        if (it->file_name == filename) {
            int first_a = it->user_p_flag[0] * 4 + it->user_p_flag[1] * 2 + it->user_p_flag[2] * 1;
            int second_a = it->extern_p_flag[0] * 4 + it->extern_p_flag[1] * 2 + it->extern_p_flag[2] * 1;
            cout << "  File: " << it->file_name << endl;
            cout << "  Size: " << diskBlock[it->d_block].block_data.length() << "  DiskBlock: " << it->d_block << "  Type: " << (it->file_type == 1 ? "regular file" : "directory") << endl;
            cout << "Access: (" << first_a << second_a << "/" << (it->file_type == 2 ? "d" : "-") << (it->user_p_flag[0] ? "r" : "-") << (it->user_p_flag[1] ? "w" : "-") << (it->user_p_flag[2] ? "x" : "-") << (it->extern_p_flag[0] ? "r" : "-") << (it->extern_p_flag[1] ? "w" : "-") << (it->extern_p_flag[2] ? "x" : "-") << ") ";
            cout << "Uid: (" << it->f_uid << " / " << getUsernameByUid(it->f_uid) << ")" << endl;
            cout << "Create:" << it->create_time.year << "-" << it->create_time.month << "-" << it->create_time.day << " " << it->create_time.hour << ":" << it->create_time.minute << ":" << it->create_time.second << endl;
            cout << "Modify:" << it->modify_time.year << "-" << it->modify_time.month << "-" << it->modify_time.day << " " << it->modify_time.hour << ":" << it->modify_time.minute << ":" << it->modify_time.second << endl;
            return 0;
        }
    }
    cout << "stat: " << filename << ": No such file or directory" << endl;
    return 0;
}

int help() {
    cout << "Operating System Filesystem Experimental Demo By LZX, version 0.1.3.221125" << endl;
    cout << "Available commands are as follows:" << endl;
    cout << "ls [OPTION] \t\t\t\t List files of the current directory.\n";
    cout << "cd [OPTION] \t\t\t\t Change user working directory.\n";
    cout << "touch [filename] \t\t\t Create a file.\n" ;
    cout << "mkdir [directory_name] \t\t\t Create a directory.\n";
    cout << "rm [Args] \t\t\t\t Remove a file or directory.\n";
    cout << "useradd \t\t\t\t User add program.\n";
    cout << "su [username] \t\t\t\t Change to another user.\n";
    cout << "chmod [filename] [permission_code] \t Change a file / directory access permission.\n";
    cout << "chown [filename] [username] \t\t Change file ownership to the given user.\n";
    cout << "cat [filename] \t\t\t\t Output a file's content.\n";
    cout << "echo [filename] [content] \t\t Append content to a given file.\n";
    cout << "stat [filename] \t\t\t Show the properties of the given file.\n";
    cout << "lsuser \t\t\t\t\t Show the users that currently exist in the system.\n";
    cout << "lsdisk \t\t\t\t\t Show the current disk usage in the system.\n";
    cout << "demo \t\t\t\t\t Generate demonstrate file structure, default permission.\n";
    cout << "clear \t\t\t\t\t Clear the output on the screen.\n";
    cout << "help \t\t\t\t\t Show this message. \n";
    cout << "exit \t\t\t\t\t Exit system." << endl;
    return 0;
}

int generateDemonstrate() {
    if (hasUser) {
        cout << "Demonstrate program running.." << endl;
        touch(userList[userPointer].username + "_testfile1");
        cout << "generated file: " << userList[userPointer].username + "_testfile1" << endl;
        echo(userList[userPointer].username + "_testfile1","this is a " + userList[userPointer].username + " generated demo file.");
        mkdir("dir1_lv1");
        cout << "generated level 1 directory: dir1_lv1" << endl;
        mkdir("dir2_lv1");
        cout << "generated level 1 directory: dir2_lv1" << endl;
        cd("dir1_lv1");
        cout << "entered dir1_lv1" << endl;
        touch(userList[userPointer].username + "_testfile2");
        echo(userList[userPointer].username + "_testfile2","this is a " + userList[userPointer].username + " generated demo file.");
        cout << "generated file: " << userList[userPointer].username + "_testfile2" << endl;
        mkdir("dir3_lv2");
        cout << "generated level 2 directory: dir2_lvl1" << endl;
        cd("dir3_lv2");
        cout << "entered dir3_lvl1" << endl;
        touch(userList[userPointer].username + "_testfile3");
        echo(userList[userPointer].username + "_testfile3","this is a " + userList[userPointer].username + " generated demo file.");
        cd("..");
        cd ("..");
        cout << "generate done" << endl;
    } else {
        cout << "No user detected, abort" << endl;
    }
    return 0;
}