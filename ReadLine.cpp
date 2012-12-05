#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <cstdlib>
#include "ReadLine.hpp"
#include <unistd.h>
using namespace std;
namespace{
  char *_readline(const char *s){
    return readline(s);
  }
}
string ReadLine::history_file;
void ReadLine::init(string history_name, bool home){
  using_history();
  history_file = history_name;
  if(home)history_file = string(getenv("HOME")) + "/" + history_file;
  read_history(history_file.c_str());

  ReadLine::eof = false;
}

string ReadLine::readline(string prompt){
  char *res = _readline(prompt.c_str());
  if(res){
    add_history(res);
    string r(res);
    free(res);
    return r;
  }else{
    ReadLine::eof = true;
    return "";
  }
}

bool ReadLine::is_eof(){
  return eof;
}
void ReadLine::finalize(){
  write_history(history_file.c_str()); 
} 

