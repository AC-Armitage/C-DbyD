#include <iostream>
#include <sodium.h>
#include <sqlite3.h>
#include <cstring>
#include <fstream>

void Help(){
    using namespace std;
    cout << "Usage:" << endl;
    cout << "\t-v, --verify            <name> <password>   | checks password against stored hash" << endl;
    cout << "\t-a, --add            <name> <password>   | adds a new entry to the database" << endl;
    cout << "\t-d, --delete         <name>              | delets an entry" << endl;
    cout << "\t-e, --edit           <name> <password>   | modify an entry password" << endl;
    cout << "\t-c, --create                             | create database\n" << endl;
}
void CreateDB(sqlite3* DB){
    using namespace std;
    cout << "Creating database..." << endl;
    int pwdb = sqlite3_open("pwdb.db", &DB);
    if (pwdb != SQLITE_OK){
        cout << "Error creating database!" << endl;
        sqlite3_close(DB);
    }
    const char *sql = "CREATE TABLE PASSWORDMAP("\
            "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL ,"\
            "NAME  TEXT NOT NULL,"\
            "PASSWORD TEXT NOT NULL,"\
            "SALT TEXT NOT NULL);";

    cout << "Creating table..." << endl;
    pwdb = sqlite3_exec(DB, sql, 0, 0,0);
    if (pwdb != SQLITE_OK){
        cout << "Could not create table!\n" << sqlite3_errmsg(DB) << endl;
    }
    sqlite3_close(DB);
}
int Verify(char* name, sqlite3* DB, std::string password){
        using namespace std;
        char hexhash[crypto_pwhash_BYTES_MIN * 2 + 1];
        const char* hexsalt;
        const char* passhash;
        cout << "Verifiying" << endl;
        int pwdb = sqlite3_open("pwdb.db", &DB);
        if (pwdb != SQLITE_OK){
            cout << "Error opening database!" << endl;
            sqlite3_close(DB);
        }
        unsigned char salt[crypto_pwhash_SALTBYTES];
        const char* sql = "SELECT PASSWORD, SALT FROM PASSWORDMAP WHERE NAME = ?";
        sqlite3_stmt* stmt;
        int sqop = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
        if (sqop != SQLITE_OK){
            std::cerr  << sqlite3_errmsg(DB) << endl;
            return -3;
        }
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        while ((sqop = sqlite3_step(stmt)) == SQLITE_ROW){
            passhash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            hexsalt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            cout << "N: " << passhash << endl;
        }
        sodium_hex2bin(salt, sizeof(salt), hexsalt, strlen(hexsalt), nullptr, nullptr, nullptr);
        string saltedpwd = password + hexsalt;
        unsigned char hash[crypto_pwhash_BYTES_MIN];
        if (crypto_pwhash(hash, sizeof(hash), saltedpwd.c_str(), sizeof(crypto_pwhash_BYTES_MIN), salt, crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0){
            cerr << "Could not verify hash" << endl;
        }
        sodium_bin2hex(hexhash, sizeof(hexhash), hash, sizeof(hash));
        cout << hexhash << endl;
        cout << passhash << endl;
        if (sqop != SQLITE_DONE){
            cerr << sqlite3_errmsg(DB) << endl;
            return -5;
        }
        sqlite3_finalize(stmt);
        sqlite3_close(DB);
       if (memcmp(hexhash, passhash, crypto_pwhash_BYTES_MIN) != 0){
            cout << "No match" << endl;
            return -1;
        }
        return 0;
    }
int main(int argc, char* argv[]){
    using namespace std;
    if(argc == 1 || argc > 4){
        Help();
        exit(1);
    }
    if (sodium_init() != 0){
        std::cerr << "Could not initiat libraries\n Exiting..." << std::endl;
        exit(-1);
    }
    sqlite3* DB;
    ifstream dbfile("pwdb.db");
    if (!dbfile.good() && strcmp(argv[1], "-c") != 0){
        cout << "No database found use -c or --create to create one!" << endl;
        exit(2);
    }

    if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--create") == 0){
        if(dbfile.good()){
            cout << "Database already exist!!" << endl;
            exit(3);
        }
        cout << "Hi" << endl;
        CreateDB(DB);
    }
    if (strcmp(argv[1], "-a") == 0){
        cout << "Adding..." << endl;
        int pwdb = sqlite3_open("pwdb.db", &DB);
        if (pwdb != SQLITE_OK){
            cout << "Error creating database!" << endl;
            sqlite3_close(DB);
        }
        unsigned char salt[crypto_pwhash_SALTBYTES];
        randombytes_buf(salt, sizeof(salt));
        char hexsalt[crypto_pwhash_BYTES_MIN * 2 + 1];
        sodium_bin2hex(hexsalt, sizeof(hexsalt), salt ,sizeof(salt));
        string password = argv[3];
        string saltedpwd = password + hexsalt;
        unsigned char hash[crypto_pwhash_BYTES_MIN];
        if (crypto_pwhash(hash, sizeof(hash), saltedpwd.c_str(), sizeof(crypto_pwhash_BYTES_MIN), salt, crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_argon2id_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0){
            cerr << "Could not perform hashing!" << endl;
        }
        password.clear();
        char hexhash[crypto_pwhash_BYTES_MIN * 2 + 1];
        sodium_bin2hex(hexhash, sizeof(hexhash), hash, sizeof(hash));
        sqlite3_stmt* stmt;

        const char *sql = "INSERT INTO PASSWORDMAP (NAME, PASSWORD, SALT) VALUES (?, ?, ?)";
        int sqop = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
        if (sqop != SQLITE_OK){
            cout << "Faild to prepare " << endl;
            sqlite3_close(DB);
        }
        sqlite3_bind_text(stmt, 1, argv[2], -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, hexhash, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, hexsalt, -1, SQLITE_STATIC);
        
        sqop = sqlite3_step(stmt);
        if (sqop != SQLITE_DONE){
            cerr << "Error" <<  sqlite3_errmsg(DB) << endl;
            exit(-2);
        }
        cout << hexhash << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(DB);
    } else if (strcmp(argv[1], "-v") == 0){
        char* name = argv[2];
        string password = argv[3];
        if (Verify(name, DB, password) == 0){
        }
    }
    
    return 0;
