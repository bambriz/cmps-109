// $Id: main.cpp,v 1.6 2014-07-09 11:50:34-07 - - $
//Bryan Ambriz

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

typedef xpair<string,string> str_str_pair;
typedef listmap<string,string> str_str_map;
//using str_str_pair = xpair<const string,string>;
//using str_str_map = listmap<string,string>;

void scan_options (int argc, char** argv) { 
   opterr = 0; 
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
      case '@':
         traceflags::setflags (optarg);
         break;
      default:
         complain() << "-" << (char) optopt << ": invalid option"
         << endl;
         break;
      }
   }
}

//trim whitespace
string trim(string& str){
   size_t pos = str.find_first_not_of(" ");
   if(pos != string::npos)
   str = str.substr(pos,str.size());
   pos = str.find_last_not_of(" ");
   if(pos != string::npos)
   str = str.substr(0,pos+1);
   else
   str.clear();
   return str;
}


bool split(string& line, string& key, string& value){
  
   size_t pos = line.find_first_of("=",0);
   if (pos != string::npos){
      key = line.substr(0,pos);
      value = line.substr(pos+1,line.size());
      return true;
   }
   return false;
}

void parsefile(const string& infilename, istream &infile){ 
   string line = "";
   string trimline = "";
   string Key = "";
   string Value = "";
   str_str_map test;
   int counter = 0;
   while(getline(infile,line)){
      counter++;
      cout << infilename<<": " << counter << ": " << line << endl;
      if(trim(line)[0] == '#') continue;
      if(split(line, Key, Value)){
         trim(Key);
         trim(Value);
         bool keyblank = Key.compare("") == 0;
         bool valueblank = Value.compare("") == 0;
         if(!keyblank && !valueblank){
            //key=value
            str_str_pair pair (Key,Value);
           
            test.insert (make_pair(Key,Value));
            cout << Key << " = " << Value << endl;
         }else if(keyblank && !valueblank){
            //=value
            for(auto itor = test.begin(); itor != test.end(); ++itor)
            if(itor->second.compare(Value) == 0)
            cout << itor->first << " = " << itor->second <<endl;
         }else if(!keyblank && valueblank){
            //key=
            test.erase(test.find(Key));
         }else if(keyblank && valueblank){
            //=
            for(auto itor = test.begin(); itor != test.end(); ++itor)
            cout << itor->first << " = " << itor->second <<endl;
         }
      }else{
         trim(line);
         if(line.size() == 0) continue;
         auto find = test.find(line);
         if(find != test.end()){
            cout << find->first << " = " << find->second <<endl;
         }else cout << line <<": key not found" <<endl;      
      }      
   }
}

int main (int argc, char** argv) {
   int exit_status = 0;
   sys_info::set_execname (argv[0]); 
   scan_options (argc, argv);
   if(optind == argc) {
      parsefile("-",cin);
   }else{
      for(int i = optind; i < argc ; ++i){
         const string infilename = argv[i];
         ifstream infile (infilename.c_str());
         if (infile.fail()) {
            syscall_error(infilename);
            exit_status = 1;
            continue;
         }
         parsefile (infilename, infile);
         infile.close();
      }
   }
   return exit_status;
}
