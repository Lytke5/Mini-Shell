#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <string>
#include <cstdio>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <csignal>

using namespace std;

time_t start;

void signalHandler(int signum)
{
    time_t end = time(nullptr);
    double elapsed_time = difftime(end, start);
    int hours = elapsed_time / 3600;
    elapsed_time -= hours * 3600;
    int minutes = elapsed_time / 60;
    elapsed_time -= minutes * 60;
    cout << "time elapsed: " << hours << "h " << minutes << "m " << elapsed_time << "s";
    exit(signum);
}


int main(){
    bool isBackground;
    start = time(nullptr);

    signal(SIGINT, signalHandler);


    while(true){
        isBackground = false;
        cout << ">";
        string input;
        getline(cin, input);
        vector<string> arguments;

        istringstream iss(input);
        string word;


        while (iss >> word) {
            arguments.push_back(word);
        }

        if(arguments[0] == "exit" && arguments.size() == 1){
            char wantToLeave;
            bool success = false;
            while(!success)
            {
                cout << "do really want to quit (y|n): ";
                cin >> wantToLeave;
                if(wantToLeave == 'y')
                    return 0;
                else if(wantToLeave == 'n')
                    success= true;
            }
        }
        else
        {
            int pid = fork();

            if(pid == 0){
                if(arguments[0] == "date" && arguments.size() < 3){
                    time_t now = time(NULL);
                    if(arguments.size() == 2 && arguments[arguments.size()-1] == "&"){
                        isBackground = true;
                        int p = fork();
                        if(p == 0)
                        {
                            cout << "[" << getpid() << "]" << endl;
                            tm* local_time = localtime(&now);
                            char date_str[100];
                            strftime(date_str, sizeof(date_str), "%a %b %d %H:%M:%S %Z %Y", local_time);
                            cout << date_str << endl;
                            cout << "done date " << endl;
                            cout << ">";
                        }
                    }
                    else
                    {
                        tm* local_time = localtime(&now);
                        char date_str[100];
                        strftime(date_str, sizeof(date_str), "%a %b %d %H:%M:%S %Z %Y", local_time);
                        cout << date_str << endl;
                    }
                }
                else if(arguments[0] == "pwd" && arguments.size() < 3){
                    char cwd[1024];
                    if(arguments.size() == 2 && arguments[arguments.size()-1] == "&"){
                        isBackground = true;
                        int p = fork();
                        if(p == 0)
                        {
                            cout << "[" << getpid() << "]" << endl;
                            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                                cout << cwd << endl;
                            }
                            cout << "done pwd " << endl;
                            cout << ">";
                        }
                    }
                    else if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                        cout << cwd << endl;
                    }
                }
                else if(arguments[0] == "echo"){
                    if(arguments[arguments.size()-1] == "&"){
                        isBackground = true;
                        int p = fork();
                        if(p == 0)
                        {
                            cout << "[" << getpid() << "]" << endl;
                            for(int i = 1; i < arguments.size(); i++){
                                cout << arguments[i] << " ";
                            }
                            cout << "done echo" << endl;
                            cout << ">";
                        }
                    }
                    else
                        for(int i = 1; i < arguments.size(); i++){
                            cout << arguments[i] << " ";
                        }
                    cout << endl;
                }
                else if(arguments[0] == "sleep" && arguments.size() < 4){
                    int time = stoi(arguments[1]);
                    if(arguments.size() == 3 && arguments[arguments.size()-1] == "&"){
                        isBackground = true;
                        int p = fork();
                        if(p == 0)
                        {
                            cout << "[" << getpid() << "]" << endl;
                            sleep(time);
                            cout << "done sleep "<< time << endl;
                            cout << ">";
                        }
                    }
                    else
                        sleep(time);
                }
                else if(arguments[0] == "ps" && arguments.size() < 3){
                    if(arguments.size() == 2 && arguments[arguments.size()-1] == "&"){
                        isBackground = true;
                        int p = fork();
                        if(p == 0)
                        {
                            cout << "[" << getpid() << "]" << endl;
                            FILE* stream = popen("ps", "r");
                            if (stream == nullptr){
                                perror("popen() failed");
                                return 0;
                            }

                            char buffer[256];
                            while (fgets(buffer, sizeof(buffer), stream)){
                                printf("%s", buffer);
                            }

                            pclose(stream);
                            cout << "done ps" << endl;
                            cout << ">";
                        }
                    }
                    else
                    {
                        FILE* stream = popen("ps", "r");
                        if (stream == nullptr){
                            perror("popen() failed");
                            return 0;
                        }

                        char buffer[256];
                        while (fgets(buffer, sizeof(buffer), stream)){
                            printf("%s", buffer);
                        }

                        pclose(stream);
                    }
                }
                else{
                    cout << "minishell: command not found: " << arguments[0] << endl;
                }
                exit(0);
            }
            else
            {
                if(!isBackground)
                {
                    wait(0);
                }
            }
        }
    }
    return 0;
}

