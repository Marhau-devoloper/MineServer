#include "curl/curl.h"
#include "iostream"
#include "vector"
#include "filesystem"
#include <string.h>

using namespace std;

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



string DownloadCore(string core, string Version, string Path) {
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
    cout << "Downloading Server Core" << endl;
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
        cout << "Download completed!" << endl;
        char *redirected_url = nullptr;
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &redirected_url);
        if (redirected_url) {
            cout << "URL Found" << endl;
        }
    }

    curl_easy_cleanup(curl);
    fclose(fp);

    return "Download finished";
}



void GenConf(string Path, string Ram){
    // start.sh path
    string Path_sh = Path + "/start.sh";
    // eula.txt path
    string Path_txt = Path + "/eula.txt";
    // data for eula
    string filed_txt = "eula=true";
    //data for start.sh
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
    //open file to writing for eula.txt
    fp = fopen(Path_txt.c_str(),"wb");
    //writing eula.txt
    fwrite(filed_txt.c_str(),sizeof(char),strlen(filed_txt.c_str()),fp);
    //close connection
    fclose(fp);
    
}




int main(){

    system("clear");
    cout << "Select Core" << "\n"  << "0 = Fabric " << "\n"  << "1 = Forge" << "\n"  << "2 = NeoForge" << "\n"  << "3 = Paper" << "\n" << "4 = PurPur" << "\n"  << "5 = Vanilla" << "\n"  << "Fabric, Forge, NeoForge is Cores With Supports a Mods"<< "\n"  << "Paper, PurPur is Plugins only Cores" << "\n" << "Vanilla is Regular Core With Not Supporting Mods or Plugins" << "\n";
    cout << "  "<< endl;
    cout << "Core : ";
    cin >> UserCore ;
    
    if ((UserCore < 0 ) || (UserCore > 5)) {
        system("clear");
        cout << "Please Choose someing between 0 and 5";
        exit(0);
    };
    system("clear");

    cout << "Core : " + Core[UserCore] << endl;
    cout << " " << endl;
    cout << "Enter Minecraft Version." <<endl;
    cout << "For Example: 1.12.2 or 1.21.0" <<endl;
    cout << "Version : ";
    cin >> Version;
    system("clear");

    cout << "Core : " + Core[UserCore] << endl;
    cout << "Version : " + Version << endl;
    cout << "  "<< endl;
    cout << "Enter server name without spaces" << endl;
    cout << "Name : ";
    cin >> Name;
    system("clear");

    cout << "Core : " + Core[UserCore] << endl;
    cout << "Version : " + Version << endl;
    cout << "Name : " + Name << endl;
    cout << "  "<< endl;
    cout << "Enter amount of ram " << endl;
    cout << "Recomend use 8 or more GB"<< endl;
    cout << "Ram : ";
    cin >> Ram;

    // construct a path 
    path = string(getenv("HOME")) + "/" + Name + "_" + Core[UserCore] + "_" + Version;
    // construct a command to create a dir inside of home 
    string cmd = "mkdir -p \"" + path + "\""; 
    // execute command
    system(cmd.c_str());
    // Download core
    DownloadCore(Core[UserCore],Version,path);
    // Create Eula And Starter
    GenConf(path,Ram);
    
    return 0;
}



