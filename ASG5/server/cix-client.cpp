// $Id: cix-client.cpp,v 1.7 2014-07-25 12:12:51-07 - - $

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstring>

using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "cix_protocol.h"
#include "logstream.h"
#include "signal_action.h"
#include "sockets.h"

logstream log (cout);

unordered_map<string,cix_command> command_map {
   {"exit", CIX_EXIT},
   {"help", CIX_HELP},
   {"ls"  , CIX_LS  },
   {"get", CIX_GET},
   {"put", CIX_PUT},
   {"rm", CIX_RM},
};

void cix_help() {
   static vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.cix_command = CIX_LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_LSOUT) {
      log << "sent CIX_LS, server did not return CIX_LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.cix_nbytes + 1];
      recv_packet (server, buffer, header.cix_nbytes);
      log << "received " << header.cix_nbytes << " bytes" << endl;
      buffer[header.cix_nbytes] = '\0';
      cout << buffer;
   }
}
bool is_port(string& port){
   if(port.size()!= 15){
        return false;
   }
   for(size_t i =1; i<=port.size(); ++i){
        if(i%4==0){
            if(port[i-1]!='.')
                return false;
        }else {
             if(port[i-1]<48 or port[i-1]>57)
                   return false;
        }
   }
   return true;
}

void cix_get(client_socket& server, string filename){
     cix_header header;
   header.cix_command = CIX_GET;
   for(size_t a = 0; a<filename.size(); ++a){ 
        header.cix_filename[a]=filename[a];
   }
    
   //strcpy(header.cix_filename,filename);
   //header.cix_filename=filename;
   log << "sending header " << header << endl;
   /*ifstream ifs (filename);
   vector<string> buffer;
   while(ifs.good()){
        string asstemp;
        getline(ifs,asstemp);
        
   }*/
   
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_FILE) {
      log << "sent CIX_LS, server did not return CIX_LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.cix_nbytes + 1];
      recv_packet (server, buffer, header.cix_nbytes);
      log << "received " << header.cix_nbytes << " bytes" << endl;
      buffer[header.cix_nbytes] = '\0';
      ofstream ofs {header.cix_filename};
      ofs.write (buffer, sizeof buffer);
      ofs.close();
      cout << header.cix_filename<<" has been created"<< endl;;
   }
}
void cix_rm(client_socket& server, string filename){
    cix_header header;
   header.cix_command = CIX_RM;
   for(size_t a = 0; a<filename.size(); ++a){ 
        header.cix_filename[a]=filename[a];
   }
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_ACK) {
      log << "sent CIX_RM, server did not return CIX_LSACK" << endl;
      log << "server returned " << header << endl;
   }else 
       log<< filename<<" has been removed."<<endl;
}
void cix_put(client_socket& server, string filename){
       cix_header header;
       header.cix_command = CIX_PUT;
       for(size_t a = 0; a<filename.size(); ++a){ 
        header.cix_filename[a]=filename[a];
   }
   ifstream ifs {header.cix_filename};
   string put_output;
   while (not ifs.eof()){
      char buffer[0x1000];
      ifs.read (buffer, sizeof buffer);
      //if (rc == nullptr) break;
      put_output.append (buffer);
   }
   ifs.close();
   header.cix_nbytes = put_output.size();
   
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   send_packet (server, put_output.c_str(), put_output.size());
   log << "sent " << put_output.size() << " bytes" << endl;
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_ACK) {
      log << "sent CIX_PUT, server did not return CIX_AK" << endl;
      log << "server returned " << header << endl;
   }
       
} 
void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

bool SIGINT_throw_cix_exit {false};
void signal_handler (int signal) {
   log << "signal_handler: caught " << strsignal (signal) << endl;
   switch (signal) {
      case SIGINT: case SIGTERM: SIGINT_throw_cix_exit = true; break;
      default: break;
   }
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   string host;
   in_port_t port;
   vector<string> args (&argv[1], &argv[argc]);
   //signal_action (SIGINT, signal_handler);
   //signal_action (SIGTERM, signal_handler);
   if (args.size() > 2) usage();
   if(args.size()== 2){
      host = get_cix_server_host (args, 0);
      port = get_cix_server_port (args, 1);
   }else if(args.size() == 1){
            if(is_port(host)){
                 host = get_cix_server_host (args, 0);
                 port = get_cix_server_port (args, 1);
          }else{
                 host = get_cix_server_host (args, 1);
                 port = get_cix_server_port (args, 0);  
             }
         }
   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         string file;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         if (SIGINT_throw_cix_exit) throw cix_exit();
         log << "command " << line << endl;
         if(line.find("ls")== string::npos and line.find("help") == string::npos and line.find("exit") == string::npos){
                size_t pos = line.find(" ")+1;
               file = line.substr(pos, line.size());
               line.erase(pos-1, line.size());
         }
         const auto& itor = command_map.find (line);
         cix_command cmd = itor == command_map.end()
                         ? CIX_ERROR : itor->second;
         switch (cmd) {
            case CIX_EXIT:
               throw cix_exit();
               break;
            case CIX_HELP:
               cix_help();
               break;
            case CIX_LS:
               cix_ls (server);
               break;
            case CIX_GET:
                cix_get(server, file);
                break;
            case CIX_PUT:
                 cix_put(server, file);
                 break;
            case CIX_RM:
                 cix_rm(server,file);
                 break;
            case CIX_ERROR:
                   log <<line <<":error dude that command breaks my balls"<< endl;
                   //throw cix_exit();
                   break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

