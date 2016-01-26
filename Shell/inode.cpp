#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "inode.h"

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

inode_t inode::getType(){
   return type;
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
   for(const auto& word : data ){
	size += word.length();
   }
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   //if() throw yshell_exn ("This is not a file!");
   return data;
}

void plain_file::writefile (const wordvec& words) {
   data.clear();
   for(const auto& word : words){
	data.push_back(word);
   }
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   size = dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

inode_state::inode_state(){  
   root = make_shared<inode>(DIR_INODE);
   cwd = root;
   directory_ptr_of(root->getContents())->addToMap(".", root);
   directory_ptr_of(root->getContents())->addToMap("..", root);
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
}
ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

/*void inode_state::changeRoot(inode_ptr root_dir){
   root = root_dir;
}*/

void inode_state::changeCWD(inode_ptr& CWD){
   cwd = CWD;
}

void inode_state::changePrompt(const string& newPrompt){
   prompt = newPrompt;
}

inode_ptr& inode_state::getRoot(){
   return root;
}

inode_ptr& inode_state::getCWD(){
   return cwd;
}

const string& inode_state::getPrompt(){
   return prompt;
}

const vector<string>& inode_state::getPath(){
   return path;
}

void inode_state::setPath(const vector<string>& newPath){
    path = newPath;
}

file_base_ptr inode::getContents(){
   return contents;
}

directory::directory(){
   dirents["."] = nullptr;
   dirents[".."] = nullptr;
}

void directory::remove(const string& filename){
   DEBUGF ('i', filename);
   if(dirents.size() == 2 || dirents.find(filename) == dirents.end()) throw yshell_exn ("File or directory \"" + filename + "\" does NOT exits!");
   if(dirents.find(filename)->second->getType() == DIR_INODE){
        if(directory_ptr_of(dirents.find(filename)->second->getContents())->getmap().size() == 2){
              directory_ptr_of(dirents.find(filename)->second->getContents())->addToMap(".", nullptr);
              directory_ptr_of(dirents.find(filename)->second->getContents())->addToMap("..", nullptr);
        }
        else 
              throw yshell_exn (filename + " directory is not empty!");
   }
   dirents.find(filename)->second = nullptr;
   dirents.erase(dirents.find(filename));
}

inode_ptr& directory::mkdir(const string& dirname){
   if(dirents.find(dirname) != dirents.end()) throw yshell_exn ("This name already exists!");
   dirents[dirname] = make_shared<inode>(DIR_INODE);
   directory_ptr_of(dirents[dirname]->getContents())->addToMap(".", dirents[dirname]);
   return dirents[dirname];
}

inode_ptr& directory::mkfile(const string& filename){
   if(dirents.find(filename) != dirents.end()) throw yshell_exn ("This name already exists!");
   dirents[filename] = make_shared<inode>(PLAIN_INODE); 
   return dirents[filename];
}

void directory::addToMap(const string& filename, inode_ptr ptr){
   dirents[filename] = ptr;
}

map<string, inode_ptr>& directory::getmap(){
   return dirents;
} 
