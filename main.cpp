#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

using namespace std;

class DB {
public:
    static DB* getInstance() {
        if (db == nullptr) {
            int ret = 0;
            db = new DB(ret); 
            if(ret == 1) {
                db = nullptr;
            }
        }
        return db;
    }
    
    int updateInternetIp(string ip, bool value) {
        if(ips.find(ip) == ips.end()) {
            cerr << "No such IP exist in DB : " << ip << endl;
            return 1;
        }
        ips[ip] = value;
        return flushIps();
    }
    
    void printDB() {
        string ip;
        bool value;
        for(auto& ipNode : ips) {
            ip = ipNode.first;
            value = ipNode.second;
            cout << ip << " : " << value << endl;
        }
    }

private:
    static DB* db; 
    string path = ""; 
    string fileName = "db.txt"; 
    unordered_map<string, bool> ips;
    
    int readFile(string& out) {
        string fPath = path + fileName;
        ifstream inFile(fPath);
        if (!inFile) {
            cerr << "Error opening file for reading!" << endl;
            return 1;
        }
        
        string line;
        while (getline(inFile, line)) {
            out += line + "\n"; 
        }
        inFile.close();
        return 0;
    }
    
    int writeFile(const string& in) {
        string fPath = path + fileName;
        ofstream outFile(fPath);
        if (!outFile) {
            cerr << "Error opening file for writing!" << endl;
            return 1;
        }
        outFile << in;
        outFile.close();
        return 0;
    }
    
    int flushIps() {
        string data = "";
        string ip;
        bool value;
        for(auto& ipNode : ips) {
            ip = ipNode.first;
            value = ipNode.second;
            data += ip + " : " + ((value) ? "1" : "0") + "\n";
        }
        return writeFile(data);
    }
    
    int parseToMap(string& in) {
        istringstream stream(in);
        string line;
    
        while (getline(stream, line)) {
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                string key = line.substr(0, colonPos);
                int value = stoi(line.substr(colonPos + 1));
    
                // Triming whitespace
                key.erase(key.find_last_not_of(" \n\r\t") + 1);
                value = stoi(line.substr(colonPos + 1));
    
                ips[key] = value;
            }
            else {
                cerr << "Invalid format of DB" << endl;
                in = "";
                ips.clear();
                return 1;
            }
        }
        return 0;
    }
    
    DB(int& iRet) {
        string data;
        int ret = readFile(data);
        if(ret != 0) {
            cerr << "Constructor failed. Cannot read the file." << endl;
            iRet = 1;
            return;
        }
        ret = parseToMap(data);
        if(ret != 0) {
            cerr << "Constructor failed. Invalid parsing." << endl;
            iRet = 1;
            return;
        }
    } 
};

DB* DB::db = nullptr;

class System {
public:
    static int changeIP(string newIp) {
        string command = "ifconfig eth0 " + newIp + " 255.255.255.0"; 
        int result = exeCommand(command); 
        if (result != 0) {
            std::cerr << "Error executing command " << result << std::endl;
            return result;
        }
        return 0;
    }
    static int reboot() {
        string command = "reboot"; 
        int result = exeCommand(command); 
        if (result != 0) {
            std::cerr << "Error executing command " << result << std::endl;
            return result;
        }
        return 0;
    }
private:
    static int exeCommand(string& command) {
        int result = system(command.c_str()); 
        return result;
    }
};

int main() {
    DB* dbInstance = DB::getInstance();
    if(dbInstance == nullptr) {
        return 0;
    }
    sleep(1);
    dbInstance->printDB();
    dbInstance->updateInternetIp("107.109.204.165", true);
    int ret = 0;
    ret = System::changeIP("107.109.205.155");
    if(ret != 0) {
        cout << "Change IP failed" << endl;
    }
    ret = System::reboot();
    if(ret != 0) {
        cout << "Reboot failed" << endl;
    }
    return 0;
}
