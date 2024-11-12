#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

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

int lsh_execute(vector<string> args) {
    
}

void lsh_loop() {
    string line = "";
    vector<string> args;
    int status = 1;

    while(status) {
        cout<<">";
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);
    }
}

int main(){
    lsh_loop();
    return 0;
}