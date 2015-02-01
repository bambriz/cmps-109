// $Id: commands.cpp,v 1.11 2014-06-11 13:49:31-07 - - $
//Bryan Ambriz
//Bambriz
#include "commands.h"
#include "debug.h"
#include <string.h>
#include <cstring>
#include <cassert>
commands::commands(): map ({
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
}){}

command_fn commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   command_map::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}


void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if ( words.size() == 1 ){
      cout << ": No such file or directory" << endl;
   }else{
      wordvec_itor itor = words.begin() + 1;
      const wordvec_itor end = words.end();
      for (int i = 1; itor != end; ++itor, ++i){
         inode* tar = path_finder(state, words, i, 0);
         if ( tar != NULL && !tar->is_dir() ){
            cout << tar->readfile() << endl;
         } else{
            cout << "cat: " << words[i] << 
            ": No such file or directory" << endl;
         }
      }
   }
        
        
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
  string error;
   if ( words.size() == 1 ){
      state.set_cwd_to_root();
   }
   else if ( words.size() == 2){
      inode* target = path_finder(state, words, 1, 0);
      if ( target == NULL ) {
         error += "cd: " + words[1] + 
            ": No such file or directory";
         TRACE ('c', "Invalid directory path");
      }else{
         state.set_cwd(target);
         TRACE ('c', "Setting cwd to path");
      }
   }else {
      error += "cd:";
      wordvec_itor itor = words.begin() + 1;
      while ( itor != words.end() ) error += " " + *itor++;
      error += ": No such file or directory";
   }
   if ( error.size() > 0)
      throw yshell_exn(error);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string output("");
   if ( words.size() > 1 ){
      wordvec_itor itor = words.begin() + 1;
      const wordvec_itor end = words.end();
      for (; itor != end; ++itor) {
         output += *itor;
         output += " ";
      }
      output.erase(output.end()-1);
   }
   cout << output << endl;
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   int ender;
   if ( words.size() > 1 ){
      ender = atoi(words[1].c_str());
      exit_status::set(ender);
   }
   throw ysh_exit_exn();
}


void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    if ( words.size() == 1 ){
      inode* cwd = state.getcwd();
      cout << cwd->name() << ":\n" << cwd << "  " << endl;
   }
   else{
      inode* target;
      wordvec_itor itor = words.begin();
      ++itor;
      wordvec_itor end = words.end();
      for (int i = 1; itor != end; ++itor, ++i){
         target = path_finder(state, words, i, 0);
         if ( target == NULL ) 
            cerr << "ls: Invalid file or directory: " 
               << words[i] << endl;
         else
            cout << words[i] << ":" << "\n" << target << "  " << endl;
      }
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    string error("");
   
   inode* head;
   if ( words.size() == 1 ){
      head = state.getcwd();
      string n;
      lsr_aux(head, n);
   }else {
      wordvec_itor itor = words.begin();
      ++itor; 
      wordvec_itor end = words.end();
      for (int i = 1; itor!=end; ++itor, ++i){
         head = path_finder(state, words, i, 0);
         if ( head != NULL ){
            string n;
            lsr_aux(head, n);
         }else {
            cout << "lsr: No such file or directory: "
               << words[i] << endl;
         }
      }
   }
}


void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    string error("");
   if ( words.size() == 1 )
      error += "make: Please specify a filename";
   else {
      inode* target_parent = path_finder(state, words, 1, 1);
      if ( target_parent != NULL ){
         bool newfile = false;
         TRACE ('c', "Attempting to add a new file: " << words
                << "\nWith target_parent:" << target_parent);
         wordvec path = split(words[1], "/" );
         string dirname(path[path.size()-1]);
         inode* targetfile = target_parent->get_child_dir(dirname);
         if ( targetfile == NULL ){
            targetfile = new inode(FILE_INODE);
            newfile = true;
            TRACE ('c', "Created a new file in: " << path);
         }
         if ( !targetfile->is_dir() ){
            targetfile->writefile(words);
            if ( newfile == true ){
               string name(words[1]);
               target_parent->add_file(dirname, targetfile);
               TRACE ('c', "Adding new file to directory...");
            }
         }else
            error += "make: Directory already exists with name: " + 
               words[1];
      }else {
         error += "make: Invalid file or directory";
      }
   }
   if ( error.size() > 0 )
      throw yshell_exn(error);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    string error("");
   if ( words.size() == 1 || words.size() > 2 ){
      error += "mkdir: Invalid directory name";
   }
   else if ( words.size() == 2){
      inode* target = path_finder(state, words, 1, 1);
      if ( target != NULL ){
         wordvec path = split(words[1],"/");
         if ( target->get_child_dir(path[path.size()-1]) != NULL )
            error += "mkdir: Directory already exists";
         else {
            inode* newDir = new inode(DIR_INODE);
            target->add_dirent(path[path.size()-1], newDir);
         }
      }else {
         error += "mkdir: Invalid path";
      }
   }
   if ( error.size() > 0 )
      throw yshell_exn(error);
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string newprompt;
   if ( words.size() > 1 ){
      wordvec_itor itor = words.begin() + 1;
      const wordvec_itor end = words.end();
      for (; itor != end; ++itor) {
         newprompt += *itor;
         newprompt += " ";
      }
      newprompt.erase(newprompt.end()-1);
   }
   state.setprompt(newprompt);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    string path("");
   path += state.getcwd()->name();
   inode* parent = state.getcwd()->get_parent();
   while ( parent->get_inode_nr() !=
           state.getroot()->get_inode_nr() ){
      path = parent->name() + "/" + path;
      parent = parent->get_parent();
   }
   if ( path.compare("/") != 0)
      path = "/" + path;
   cout << path << endl; 
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
     string error("");
   if ( words.size() == 1 || words.size() > 2 ){
      error += "rm: Invalid file or directory";
   }
   else if ( words.size() == 2){
      inode* target = path_finder(state, words, 1, 0);
      if ( target != NULL ){
         if ( ( target->is_dir() && target->size() <= 2 ) || 
             !target->is_dir() ){
            inode* target_parent = path_finder(state, words, 1, 1);
            wordvec path = split(words[1],"/");
            if ( target_parent->delete_child( path[path.size()-1] )
                == false ){
               TRACE ('c', "Cannot delete file for unknown reason" );
               error += "rm: Cannot delete '.' or '..'";
            }else 
               TRACE ('c', "Successfully deleted child from parents" );
         }else {
            error += "rm: Cannot delete a non-empty directory";
         }
      }else {
         error += "rm: Invalid file or directory";
      }
   }
   if ( error.size() > 0 )
      throw yshell_exn(error);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    string error("");
   if ( words.size() == 1 || words.size() > 2){
      error += "rmr: Invalid file or directory";
   }
   else if ( words.size() == 2){
      inode* target_head = path_finder(state, words, 1, 0);
      inode* parent = path_finder(state, words, 1, 1);
      TRACE('c', "Recursivly deleting subdirectories starting with: "
         << target_head);
      if ( target_head->is_dir() ){
         rmr_recur(target_head);
      }
      if ( parent->get_inode_nr() != target_head->get_inode_nr() )
         parent->delete_child(target_head->name());
   }
   if ( error.size() > 0 )
      throw yshell_exn(error);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

inode* path_finder(inode_state& state, const wordvec &words,
                     int path_index, int limitBy){
   bool start_at_root = false;
   wordvec path = split(words[path_index],"/");
   if (words[path_index].at(0)=='/')
      start_at_root = true;
   TRACE ('c', "path_finder(): Navigating path: " << path );
   
   inode* head;
   if ( start_at_root == true ){
      head = state.getroot();
   } else
      head = state.getcwd();
   
   int size = path.size()-limitBy;
   for ( int i = 0; i < size; ++i ){
      if ( head->is_dir() ){
         TRACE ('c', "path_finder(): head is a directory" << path );
         head = head->get_child_dir(path[i]);
      } else{
         head = NULL;
         TRACE ('c', "path_finder(): Failed, attempted to navigate \
                  path to a file: "
                  << path );
      }
      if ( head == NULL )
         return NULL;
   }
   return head;
}

void lsr_aux(inode* current_inode, string path){
   const string name = current_inode->name();
   path += current_inode->name();
   cout << path
   << ":\n" << current_inode << endl;
   directory* directory = current_inode->get_dirents();
   directory::const_iterator itor = directory->begin();
   ++itor; ++itor;
   directory::const_iterator end = directory->end();
   if ( path.compare("/") != 0 )
      path += "/";
   for (; itor != end; ++itor) {
      if ( itor->second->is_dir() ){
         lsr_aux(itor->second, path);
      }
   }
}
//unale to make work...
void rmr_recur(inode* current_inode){
   directory* directory = current_inode->get_dirents();
  // directory::iterator itor = directory->begin(); //broken idk why
  // ++itor; 
  // ++itor;
   directory::const_iterator end = directory->end();
   for (; itor != end;) {
      if ( itor->second->is_dir() )
         rmr_recur(itor->second);
      //directory->remove();
   }
   
}