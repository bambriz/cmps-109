// $Id: cix-server.cpp,v 1.7 2014-07-25 12:12:51-07 - - $

#include <iostream>
using namespace std;
#include <fstream>
#include <cstring>
#include <libgen.h>
#include <unisrd.h>

#include "cix-protocol.h"
#include "logstream.h"
#include "signal_action.h"
#include "sockets.h"

logstream log (cout);

void reply_ls (accepted_socket& client_sock, cix_header& header) {
   FILE* ls_pipe = popen ("ls -l", "r");
   if (ls_pipe == NULL) {
      log << "ls -l: popen failed: " << strerror (errno) << endl;
      header.cix_command = CIX_NAK;
      header.cix_nbytes = errno;
      send_packet (client_sock, &header, sizeof header);
   }
   string ls_output;
   char buffer[0x1000];
   for (;;) {
      char* rc = fgets (buffer, sizeof buffer, ls_pipe);
      if (rc == nullptr) break;
      ls_output.append (buffer);
   }
   header.cix_command = CIX_LSOUT;
   header.cix_nbytes = ls_output.size();
   memset (header.cix_filename, 0, CIX_FILENAME_SIZE);
   log << "sending header " << header << endl;
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, ls_output.c_str(), ls_output.size());
   log << "sent " << ls_output.size() << " bytes" << endl;
}
void reply_get (accepted_socket& client_sock, cix_header& header) {
      ifstream ifs {header.cix_filename};
     
   if (ifs.fail()) {
      log << header.cix_filename<<": ifstream failed: " << strerror (errno) << endl;
      header.cix_command = CIX_NAK;
      header.cix_nbytes = errno;
      send_packet (client_sock, &header, sizeof header);
   }
   string get_output;
   
   while (not ifs.eof()){
      char buffer[0x1000];
      ifs.read (buffer, sizeof buffer);
      //if (rc == nullptr) break;
      get_output.append (buffer);
   }
   header.cix_command = CIX_FILE;
   header.cix_nbytes = get_output.size();
   memset (header.cix_filename, 0, CIX_FILENAME_SIZE);
   log << "sending header " << header << endl;
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, get_output.c_str(), get_output.size());
   log << "sent " << get_output.size() << " bytes" << endl;  
}
void reply_put(accepted_socket& client_sock, cix_header& header){
         char buffer[header.cix_nbytes + 1];
      recv_packet (server, buffer, header.cix_nbytes);
      log << "received " << header.cix_nbytes << " bytes" << endl;
      buffer[header.cix_nbytes] = '\0';
      ofstream ofs {header.cix_filename};
      if(ofs.fail()){
            log << header.cix_filename<<": ifstream failed: " << strerror (errno) << endl;
            header.cix_command = CIX_NAK;
            header.cix_nbytes = errno;
            send_packet (client_sock, &header, sizeof header);
      }
      ofs.write (buffer, sizeof buffer);
      cout << header.cix_filename<<" has been created"<< endl; 
      header.cix_command = CIX_ACK;
      memset (header.cix_filename, 0, CIX_FILENAME_SIZE);
      header.cix_nbytes{0};
      log << "sending header " << header << endl;
      send_packet (client_sock, &header, sizeof header);
      
}

void reply_rm(accepted_socket& client_sock, cix_header& header){
     int rc = unlink(header.cix_filename);
     if(rc!=0){ 
            log << header.cix_filename<<": Error deleting file: " << strerror (errno) << endl;
            header.cix_command = CIX_NAK;
            header.cix_nbytes = errno;
            send_packet (client_sock, &header, sizeof header);
     }
     log<< header.cix_filename<<": Succesfully removed: "<< endl;
      header.cix_command = CIX_ACK;
      memset (header.cix_filename, 0, CIX_FILENAME_SIZE);
      log << "sending header " << header << endl;
      send_packet (client_sock, &header, sizeof header);
}

bool SIGINT_throw_cix_exit = false;
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
   vector<string> args (&argv[1], &argv[argc]);
   //signal_action (SIGINT, signal_handler);
  // signal_action (SIGTERM, signal_handler);
   int client_fd = args.size() == 0 ? -1 : stoi (args[0]);
   log << "starting client_fd " << client_fd << endl;
   try {
      accepted_socket client_sock (client_fd);
      log << "connected to " << to_string (client_sock) << endl;
      for (;;) {
         if (SIGINT_throw_cix_exit) throw cix_exit();
         cix_header header;
         recv_packet (client_sock, &header, sizeof header);
         log << "received header " << header << endl;
         switch (header.cix_command) {
            case CIX_LS:
               reply_ls (client_sock, header);
               break;
            case CIX_GET:
               reply_get(client_sock, header);
               break;
            case CIX_PUT:
                 reply_put(client_sock, header);
                 break;
            case CIX_RM:
                   reply_rm(client_sock, header);
                   break;
            default:
               log << "invalid header from client" << endl;
               log << "cix_nbytes = " << header.cix_nbytes << endl;
               log << "cix_command = " << header.cix_command << endl;
               log << "cix_filename = " << header.cix_filename << endl;
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

