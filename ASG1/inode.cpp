// $Id: inode.cpp,v 1.11 2014-06-20 14:03:53-07 - - $
//bryan ambriz
//bambriz
#include <iostream>
#include <stdexcept>
#include <cassert>

using namespace std;

#include "debug.h"
#include "inode.h"
#include <cstring>

int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");
   return dirptr;
}


size_t plain_file::size() const {
  size_t size {0};
   for(uint i = 0; i < data.size(); i++){
     size += data.at(i).size();
   }
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

size_t directory::size() const {
   DEBUGF ('i', "size = " << size);
   return dirents.size();
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
     map<string,inode_ptr>::iterator t = dirents.find(filename);
   if(it != dirents.end()){
     dirents.erase(t);
   }
}
//this
inode* inode::get_child_dir(const string &dirname){
   inode* target = NULL;
   if ( type == FILE_INODE ) return target;
   
   //map<string, inode*>
   directory::const_iterator itor = contents.dirents->begin();
   directory::const_iterator end = contents.dirents->end();
   for (; itor != end; ++itor) {
      if ( dirname.compare(itor->first) == 0)
         return itor->second;
   }
   return NULL;
   
}

void inode::set_parent(inode* parent){
   contents.dirents->insert( make_pair("..", parent) );
}

void inode::add_dirent(string& name, inode* addition){
   contents.dirents->insert( make_pair(name, addition));
   addition->set_parent(this);
}

void inode::add_file(string& name, inode* newfile){
   contents.dirents->insert( make_pair(name, newfile));
}
//this
//vale
const string inode::name(){
   inode* parent = get_child_dir("..");
   if ( parent == this ) return "/";
   else {
      directory::const_iterator itor =
         parent->contents.dirents->begin();
      directory::const_iterator end =
         parent->contents.dirents->end();
      for (; itor != end; ++itor) {
         if ( itor->second == this )
            return string(itor->first);
      }
   }
   return "";
}

bool inode::is_dir(){
   if (type == DIR_INODE)
      return true;
   return false;
}

directory* inode::get_dirents(){
   if ( !is_dir() ) return NULL;
   return contents.dirents;
}
//vale
//hmm
bool inode::delete_child(const string& child_name){
   //a file inode has no directory
   if ( type == FILE_INODE )
      throw yshell_exn("Cannot call delete_child() on FILE_INODE");
   if ( child_name.compare(".") == 0 ||
         child_name.compare("..") == 0 )
      return false;
   directory::iterator itor = contents.dirents->begin();
   directory::const_iterator end = contents.dirents->end();
   for (; itor != end; ++itor) {
      if ( itor->first.compare(child_name) == 0 ){
         contents.dirents->erase(itor++);
         return true;
      }
      else ++itor;
   }
   return false;
}

inode* inode::get_parent(){
   if ( type == FILE_INODE )
      throw yshell_exn("Cannot call get_parent() on FILE_INODE");
   directory::iterator p = contents.dirents->begin();
   ++p;
   return p->second;
}

ostream &operator<< (ostream &out, inode* node) {
   directory::const_iterator itor = node->contents.dirents->begin();
   directory::const_iterator end = node->contents.dirents->end();
   int i = 0;
   for (; itor != end;) {
      out << setw(6) << itor->second->get_inode_nr() << setw(6)
         << itor->second->size() << "  " << itor->first;
      if ( i < 2 ){
         ++i;
      } else if ( itor->second->is_dir() ) 
         out << "/";
      ++itor;
      if ( itor != end ) out << "\n";
   }
   return out;
}

inode_state::inode_state(): root (NULL), cwd (NULL), prompt ("%") {
   TRACE ('i', "root = " << (void*) root << ", cwd = " << (void*) cwd
          << ", prompt = " << prompt);
}

inode_state::~inode_state(){
   TRACE ('i', "root = " << (void*) root << ", cwd = " << (void*) cwd
          << ", prompt = " << prompt);
   
   delete root;
}

void inode_state::setprompt(const string &newprompt){
   prompt = newprompt;
}

string inode_state::getprompt(){
   return prompt;
}

void inode_state::create_new_file_system(){
   root = new inode(DIR_INODE);
   root->set_parent(root);
   set_cwd_to_root();
}

inode* inode_state::getcwd(){
   return cwd;
}

inode* inode_state::getroot(){
   return root;
}

void inode_state::set_cwd_to_root(){
   cwd = root;
}

void inode_state::set_cwd(inode* node){
   assert(node->is_dir());
   cwd = node;
}
//hmm 




inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}
