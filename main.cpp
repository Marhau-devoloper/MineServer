#include "curl/curl.h"
#include "iostream"
#include "vector"
#include "filesystem"
#include <string.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include "include/nlohmann/json.hpp"
using namespace std;
using namespace nlohmann;
vector<string> Core = {"Fabric","Forge","NeoForge","Paper","PurPur","Vanilla"};
string Version;
string type;
string path;
string Name = "NoName";
string Cmd;
string Ram = "8";
short int UserCore;

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}


size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}


namespace fs = std::filesystem;

// Returns the first Forge server jar in a folder
std::string findForgeJar(const std::string& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            auto filename = entry.path().filename().string();
            std::cout << "Checking: " << filename << std::endl;

            // Check if filename starts with "forge-" and ends with ".jar"
            if (filename.size() >= 10 &&
                filename.substr(0, 6) == "forge-" &&
                filename.substr(filename.size() - 4) == ".jar" &&
                filename.find("installer") == std::string::npos) // ignore installer
            {
                return filename; // first matching jar
            }
        }
    }
    return "";
}

string toLower(std::string str) {
    transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return tolower(c); });
    return str;
}
string DownloadCore(string core, string Version, string Path) {
    //lowercase core name from Forge to forge
    transform(core.begin(), core.end(), core.begin(),
              ::tolower);
    //url
    string url = "https://mcutils.com/api/server-jars/" + core + "/" + Version + "/download";
    
    //core path
    Path = Path + "/server.jar";

    const char* outfilename = Path.c_str();
    //initializing CURL, FILE
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) {
        return "Failed to initialize CURL";
    }
    cout << "Downloading Server Core: " << url << endl;
    fp = fopen(outfilename, "wb");
    if (!fp) {
        curl_easy_cleanup(curl);
        return "Failed to open output file";
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow redirects

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cerr << "CURL failed: " << curl_easy_strerror(res) << endl;
    } else {
        cout << "\033[32m" << "Download completed!" << "\033[0m" << endl;
        
        char *redirected_url = nullptr;
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &redirected_url);
        char ok = 1;
        if (redirected_url and ok != 1) {
            cout << "\033[31m" << "URL Found" << "\033[0m" << endl;
        }
    }

    curl_easy_cleanup(curl);
    fclose(fp);

    return "Download finished";
}

void GetVersions(string Core){
    CURL * handle; 
    json jsn;
    CURLcode res;
    string response;
    handle = curl_easy_init();
    
    
    string URL = "https://mcutils.com/api/server-jars/" + toLower(Core);
    curl_easy_setopt(handle,CURLOPT_URL, URL.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION,WriteCallback);
    //curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(handle,CURLOPT_WRITEDATA, &response);
    curl_easy_perform(handle);
    curl_easy_cleanup(handle); 
    jsn = json::parse(response);
    cout << Core << ": " <<endl;
    char counts = 0;
    
    cout << "" << endl;
    cout << "=====================================================================================" << endl;
    for (const auto& item : jsn){
        
        counts += 1;
        if (counts >= 11){
            cout << " " << item["version"].get<string>() << '\n';
            counts = 0;
        }else{
            cout << " " << item["version"].get<string>() << ",";
        }
        
    }
    


    cout << "" << endl;
    
    cout << "=====================================================================================" << endl;
    cout << "For Example Select Any Version Between: " << jsn.front()["version"].get<string>() << " and " << jsn.back()["version"].get<string>() <<endl;
    cout << "Enter Minecraft Version From List Above." <<endl;
    
    
    
    
        
    
    
}

void GenConf(string Path, string Ram,string Core){
    // start.sh path
    string Path_sh = Path + "/start.sh";
    // eula.txt path
    string Path_txt = Path + "/eula.txt";
    // data for eula
    string filed_txt = "eula=true";

    
    //data for forge start.sh
    if (Core == "Forge"){
        //installing server
        system(("cd "+ Path +" && java -Xmx12G -jar " + Path + "/server.jar" + " --installServer").c_str());
        //data for start.sh
        string filed_sh = "java -Xmx"+ Ram +"G -jar " + findForgeJar(Path) + " nogui"; 
        //intilizing FILE
        FILE *fp;
        //open file to writing for start.sh
        fp = fopen(Path_sh.c_str(),"wb");
        //writing start.sh
        fwrite(filed_sh.c_str(),sizeof(char),strlen(filed_sh.c_str()),fp);
        //close connection
        fclose(fp);
        // making a start.sh executeble
        system(("chmod +x " + Path_sh).c_str());
        
    }else{
        //data for regular core start.sh
        string filed_sh = "java -Xmx"+ Ram +"G -jar server.jar --nogui"; 
        //intilizing FILE
        FILE *fp;
        //open file to writing for start.sh
        fp = fopen(Path_sh.c_str(),"wb");
        //writing start.sh
        fwrite(filed_sh.c_str(),sizeof(char),strlen(filed_sh.c_str()),fp);
        //close connection
        fclose(fp);
        // making a start.sh executeble
        system(("chmod +x " + Path_sh).c_str());
    }
    
    
    FILE *fp;
    //open file to writing for eula.txt
    fp = fopen(Path_txt.c_str(),"wb");
    //writing eula.txt
    fwrite(filed_txt.c_str(),sizeof(char),strlen(filed_txt.c_str()),fp);
    //close connection
    fclose(fp);
    
}




int main(){


    
    system("clear");
    cout << "------------------ MineServer By Marhau -------------------" << endl;
    cout << "Select Core" << "\n"  << "0 = Fabric " << "\n"  << "1 = Forge" << "\n"  << "2 = NeoForge" << "\n"  << "3 = Paper" << "\n" << "4 = PurPur" << "\n"  << "5 = Vanilla" << "\n"  << "Fabric, Forge, NeoForge is Cores With Supports a Mods"<< "\n"  << "Paper, PurPur is Plugins only Cores" << "\n" << "Vanilla is Regular Core With Not Supporting Mods or Plugins" << "\n";
    cout << "-----------------------------------------------------------" << endl;
    cout << "  "<< endl;
    
    cout << "Core : ";
   
    cin >> UserCore ;
    
    if ((UserCore < 0 ) || (UserCore > 5)) {
        system("clear");
        cout << "\033[31m" << "Please Choose Value between 0 and 5" << "\033[0m" << endl;
        exit(0);
    };
    system("clear");

    cout << "Core : " + Core[UserCore] << endl;
    cout << " " << endl;
    cout << "List of Versions" << endl;
    GetVersions(Core[UserCore]);
    
    cout << "Version : ";
    cin >> Version;
    
    system("clear");
    cout << "-----------------" << endl;
    cout << "Core : " + Core[UserCore] << endl;
    cout << "Version : " + Version << endl;
    cout << "-----------------" << endl;
    cout << "  "<< endl;
    cout << "Enter server name without spaces" << endl;
    cout << "Name : ";
    cin >> Name;
    system("clear");
    cout << "-----------------" << endl;
    cout << "Core : " + Core[UserCore] << endl;
    cout << "Version : " + Version << endl;
    cout << "Name : " + Name << endl;
    cout << "-----------------" << endl;
    cout << "  "<< endl;
    cout << "Enter amount of ram " << endl;
    cout << "Recommended to use 8 or more GB of Ram"<< endl;
    cout << "Ram : ";
    cin >> Ram;
    system("clear");
    // construct a path 
    path = string(getenv("HOME")) + "/" + Name + "_" + Core[UserCore] + "_" + Version;
    // construct a command to create a dir inside of home 
    string cmd = "mkdir -p \"" + path + "\""; 
    // execute command
    system(cmd.c_str());
    // Download core
    DownloadCore(Core[UserCore],Version,path);
    // Create Eula And Starter
    GenConf(path,Ram,Core[UserCore]);
    system("clear");
    cout << "Core : " + Core[UserCore] << endl;
    cout << "Version : " + Version << endl;
    cout << "Ram : " + Ram + "GB"<< endl;
    cout << "Name : " + Name << endl;
    cout << "-----------------------------------------------------------" << endl;
    cout << "\033[32m" << "Server Created At "<< path << "\033[0m" << endl;
    cout << "-----------------------------------------------------------" << endl;
    
    return 0;
}



