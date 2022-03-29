//George Rush 2022 | github.com/gmrush

#include "server.h"
#include "client.h"

//#include "cryptopp/cryptlib.h"
//#include "cryptopp/sha.h"

using namespace std;

void init() {
    string type, hostname, port, password;
    fstream fin;
    vector<string> vec;

    fin.open("config.cfg", fstream::in);
    if (fin.is_open()) {
        string line;
        while (getline(fin, line)){
            if (line[0] == '#' || line.empty()) continue;
            string str;
            stringstream ssin(line);
            while (ssin.good()) {
                ssin >> str;
                vec.push_back(str);
            }
        }
        fin.close();
    }

    type = vec[1];
    if (type == "server") {
        hostname = vec[3];
        port = vec[5];
        password = vec[7];
        Server serv = Server(hostname, port, password);
        serv.run();
    } else {
        Client cl = Client();
        cl.run();
        exit(0);
    }
}

string pad(string str) { //Pad strings to create uniform configuration file
    int offset = 16 - str.length();
    while (offset >= 4) {
        str += '\t';
        offset -= 4;
    }
    for (int i = 0; i < offset; i++) str += ' ';
    return str;
}

const string currentDateTime() { //From user TrungTN via StackOverflow
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

void setupClient() {
    string in, str;
    fstream fin;   
    {
        str = "## Configuration file for Cleareye\n## Generated on " + currentDateTime() + "\n\n"; 
        fin.open("config.cfg", fstream::out);
        fin.write(str.c_str(), str.length());
        str = pad("type") + "client" + '\n';
        fin.write(str.c_str(), str.length());
    }
    fin.close();
    init();
}

void setupServer() { //Setup configuration file for server
    string in, str;
    fstream fin;
    {
        str = "## Configuration file for Cleareye\n## Generated on " + currentDateTime() + "\n\n"; 
        fin.open("config.cfg", fstream::out);
        fin.write(str.c_str(), str.length());
        str = pad("type") + "server" + '\n';
        fin.write(str.c_str(), str.length());
    }
    {
        cout << "[?] Enter a hostname for the server:\n[-] ";
        getline(cin, in);
        if (in.empty()) in = "localhost";
        str = pad("hostname") + in + '\n';
        fin.write(str.c_str(), str.length());
    }
    {
        cout << "[?] Enter the port to connect on (default 999):\n[-] ";
        getline(cin, in);
        if (in.empty()) in = "999";
        str = pad("port") + in + '\n';
        fin.write(str.c_str(), str.length());
    }
    { 
        cout << "[?] Enter a password (used for rcon, default 'cleareye'):\n[-] ";
        getline(cin, in);
        if (in.empty()) in = "cleareye";

        /*CryptoPP::SHA256 hash;
        string digest;
        hash.Update( (const BYTE*) in.data(), in.size());
        digest.resize(hash.DigestSize());
        hash.Final( (BYTE*) &digest[0]);*/
        

        str = pad("password") + in + '\n';
        fin.write(str.c_str(), str.length());
    }
    fin.close();
    init();
}

int main(int argc, char* argv[]) {
    string logo = "=========================================\n"
                       "   ________\n"                            
                       "  / ____/ /__  ____ _________  __  _____\n"
                       " / /   / / _ \\/ __ `/ ___/ _ \\/ / / / _ \\\n"
                       "/ /___/ /  __/ /_/ / /  /  __/ /_/ /  __/\n"
                       "\\____/_/\\___/\\__,_/_/   \\___/\\__, /\\___/\n"
                       "                            /____/\n"
                       "=========================================\n\n";
    cout << logo;
    cout << "[INFO] Version " << VERSION << " by " << AUTHOR << '\n';
    fstream fin;
    fin.open("config.cfg", fstream::in);
    if (fin.is_open()) {
        fin.close();
        init();
    }
    else {
        cout << "[INFO] No config file found, running first time setup\n[?] Setup this system as a server (s) or client (c)?\n[-] ";
        string in;
        getline(cin, in);
        if (in == "c") setupClient();
        else if (in == "s") setupServer();
    }
    return 0;
}
