#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <unordered_map>
#include <limits.h>
#include <functional>

using namespace std;

int lsh_cd(vector<char*> args);
int lsh_help(vector<char*> args);
int lsh_exit(vector<char*> args);
int lsh_pwd(vector<char*> args);

unordered_map<string, function<int(vector<char*>)> > built_in_commands = {
    {"cd", lsh_cd},
    {"help", lsh_help},
    {"exit", lsh_exit},
    {"pwd", lsh_pwd}
};

int lsh_exit(vector<char*> args) {
    return 0;
}

int lsh_help(vector<char*> args) {
    cout<<"ss varma shell"<<endl;
    cout<<"Type program name, arguments with space separation and hit enter"<<endl;
    cout<<"Following are the built-in commands"<<endl;
    for (auto command : built_in_commands) {
        cout<<command.first<<endl;
    }
    return 1;
}

int lsh_cd(vector<char*> args) {
   if (args.size() < 2 || args[1] == NULL) {
     cerr<<"Expected argument to cd command"<<endl;
   } else if (chdir(args[1]) != 0) {
    perror("lsh");
   }
   return 1;
}

int lsh_pwd(vector<char*> args) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, PATH_MAX) != NULL) {
    cout<<cwd<<endl;
  } else {
    perror("lsh");
  }
  return 1;
}

string lsh_read_line(){
  string line;
  getline(cin, line);
  return line;  
}

vector<string> lsh_split_line(string line) {
    bool is_quote_seen = false;
    //to store single or double quote
    char quote_char; 
    vector<string> tokens;
    string token="";
    int len = line.length();
    for(int i = 0; i < len; ++i) {
        char ch = line[i];
        if ((ch == '\'' || ch == '"') && !is_quote_seen) {
            is_quote_seen = true;
            quote_char = ch; 
        } else if (ch == quote_char && is_quote_seen) {
            is_quote_seen = false;
            quote_char='\0';
        } else if (ch == '\\' && i + 1 < len) {
            char next = line[++i];
            if (next == 'n') token += '\n';
            else if (next == 't') token += '\t';
            else if (next == '\\') token += '\\';
            else if (next == '"') token += '"';
            else if (next == '\'') token += '\'';
            else token += next;
        } else if (!is_quote_seen && ch == ' '){
            if(!token.empty()){
                tokens.push_back(token);
                token="";
            }
        } else{
            token += ch;
        }
    }
    if(!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

int lsh_launch(vector<char*> args) {

    pid_t pid = fork();
    if (pid == 0) {  //child process
        if (execvp(args[0], args.data()) == -1) {
            perror("error");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("fork failed");
    } else { //parent process
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status)); 
    }

    return 1;
}

int lsh_execute(vector<char*> args) {
    if (args.size() == 0) {
        return 1;
    }
    if (args[0] == NULL) {
        return 1;
    }
    string command = args[0]; 
    if (built_in_commands.find(command) != built_in_commands.end()) {
        return built_in_commands[command](args);
    }
    
    return lsh_launch(args);
}

vector<char*> convert_args_to_c_args(vector<string> args) {
    vector<char*> c_args;
    for (auto arg : args) {
        c_args.push_back(strdup(arg.c_str()));
    }
    c_args.push_back(nullptr);
    return c_args;
}

void free_c_args(vector<char*> c_args) {
    for (auto& arg : c_args) {
        free(arg);
    }
}

void lsh_loop() {
    string line = "";
    vector<string> args;
    vector<char*> c_args;
    int status = 1;

    while(status) {
        cout<<">";
        line = lsh_read_line();
        args = lsh_split_line(line);
        c_args = convert_args_to_c_args(args); 
        status = lsh_execute(c_args);
        free_c_args(c_args);
    }
}

int main(){
    lsh_loop();
    return 0;
}