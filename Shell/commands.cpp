#include "commands.h"
#include "debug.h"

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
   
   if(words.size() != 2) throw yshell_exn ("WRONG number of arguments!");
   wordvec path = split(words.at(1), "/");
   map<string, inode_ptr> dirents = directory_ptr_of(state.getCWD()->getContents())->getmap();
   auto itor = path.cbegin();
   for(; itor != path.cend()-1; ++itor){
        if(dirents.find(*itor) != dirents.end() && dirents.find(*itor)->second->getType() == DIR_INODE)
              dirents = directory_ptr_of(dirents.find(*itor)->second->getContents())->getmap();
        else
              throw yshell_exn (*itor + " does NOT exist!");
   }
   if(dirents.find(*itor) != dirents.end()){
        if(dirents.find(*itor)->second->getType() == PLAIN_INODE){
              for(const auto& word : plain_file_ptr_of(dirents.find(*itor)->second->getContents())->readfile())
                    cout << word << " ";
              cout << endl;
        }
   }
   else
        throw yshell_exn ("There is no such a file!");
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   vector<string> pwd {"/"};
   if(words.size() == 1){
        state.changeCWD(state.getRoot());
        state.setPath(pwd);
	return;
   }
   pwd = state.getPath();
   if(words.size() > 2) throw yshell_exn ("Wrong number of argumetns!");
   wordvec path = split(words.at(1), "/");
   map<string, inode_ptr> dirents = directory_ptr_of(state.getCWD()->getContents())->getmap();
   auto itor = path.cbegin();
   for(; itor != path.cend(); ++itor){
        if(dirents.find(*itor) != dirents.end() && dirents.find(*itor)->second->getType() == DIR_INODE){
              state.changeCWD(dirents.find(*itor)->second);
              dirents = directory_ptr_of(dirents.find(*itor)->second->getContents())->getmap();
              if(*itor == "..")
                   pwd.pop_back();
              else if (*itor != ".")
                   pwd.push_back(*itor);
        }
        else {
              
              throw yshell_exn (*itor + " does NOT exist!");
        }
   }
   state.setPath(pwd);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   for(auto itor = words.cbegin() + 1 ; itor != words.cend(); ++itor)
	cout << *itor << " ";
   cout << endl;
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit_exn();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if(words.size() > 2) throw yshell_exn ("Wrong number of argumetns!");
   map<string, inode_ptr> dirents = directory_ptr_of(state.getCWD()->getContents())->getmap();
   if(words.size() > 1){
        wordvec path = split(words.at(1), "/");
        auto itor = path.cbegin();
        for(; itor != path.cend(); ++itor){
              if(dirents.find(*itor) != dirents.end() && dirents.find(*itor)->second->getType() == DIR_INODE){
                    dirents = directory_ptr_of(dirents.find(*itor)->second->getContents())->getmap();
              }
              else
                    throw yshell_exn (*itor + " does NOT exist!");
        }
   }
   if(words.size() == 1)
      cout << state.getPath();
   else
      cout << words.at(1);
   cout << ": " << endl;
   for(const auto& dirent : dirents){
      cout << "\t" << dirent.second->get_inode_nr() << "\t";
      if(dirent.second->getType() == PLAIN_INODE)
          cout << plain_file_ptr_of(dirent.second->getContents())->size(); 
      else 
          cout << directory_ptr_of(dirent.second->getContents())->size();
      cout << "\t" << dirent.first;
      if(dirent.second->getType() == DIR_INODE && dirent.first != "." && dirent.first != "..")
          cout << "/";
      cout << endl; 
   }   
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}


void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   if(words.size() == 1) throw yshell_exn ("No arguments!");
   wordvec word = words;
   word.erase(word.begin(), word.begin()+2);
   wordvec path = split(words.at(1), "/");
   if(path.size() == 1){
        plain_file_ptr_of(directory_ptr_of(state.getCWD()->getContents())->mkfile(words.at(1))->getContents())->writefile(word);
        return;
   }
   map<string, inode_ptr> dirents = directory_ptr_of(state.getCWD()->getContents())->getmap();
   auto itor = path.cbegin();
   for(; itor != path.cend()-1; ++itor){
        if(dirents.find(*itor) != dirents.end() && dirents.find(*itor)->second->getType() == DIR_INODE)
              dirents = directory_ptr_of(dirents.find(*itor)->second->getContents())->getmap();
        else
              throw yshell_exn (*itor + " does NOT exist!");
   }
   plain_file_ptr_of(directory_ptr_of(dirents.find(".")->second->getContents())->mkfile(*itor)->getContents())->writefile(word);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if(words.size() == 1) throw yshell_exn ("No arguments!");
   wordvec path = split(words.at(1), "/");
   if(path.size() == 1){
        directory_ptr_of(state.getCWD()->getContents())->mkdir(words.at(1));
        directory_ptr_of(directory_ptr_of(state.getCWD()->getContents())->getmap().find(words.at(1))->second->getContents())->addToMap("..", state.getCWD()); 
        return;
   } 
   map<string, inode_ptr> dirents = directory_ptr_of(state.getCWD()->getContents())->getmap();
   auto itor = path.cbegin();
   for(; itor != path.cend()-1; ++itor){
        if(dirents.find(*itor) != dirents.end() && dirents.find(*itor)->second->getType() == DIR_INODE)
              dirents = directory_ptr_of(dirents.find(*itor)->second->getContents())->getmap();
        else
              throw yshell_exn (*itor + " does NOT exist!");
   }
   directory_ptr_of(dirents["."]->getContents())->mkdir(*itor);
   directory_ptr_of(directory_ptr_of(dirents["."]->getContents())->getmap().find(*itor)->second->getContents())->addToMap("..", dirents["."]); 
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string str= "";
   for(auto itor = words.cbegin() + 1 ; itor != words.cend(); ++itor)
 	str += *itor + " ";
   state.changePrompt(str);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(state.getPath().size() == 1)
        cout << "/";
   else
        for(auto itor = state.getPath().cbegin() + 1; itor != state.getPath().cend(); ++itor)
             cout << "/" << *itor ;
   cout << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words); 

   if(words.size() == 1) throw yshell_exn ("No arguments!");
   wordvec path = split(words.at(1), "/");
   if(path.back() != "." && path.back() != ".."){
        if(path.size() == 1)
              directory_ptr_of(state.getCWD()->getContents())->remove(path[0]);
        else {
              map<string, inode_ptr> dirents = directory_ptr_of(state.getCWD()->getContents())->getmap();
              auto itor = path.cbegin();
              for(; itor != path.cend()-1; ++itor){
                     if(dirents.find(*itor) != dirents.end() && dirents.find(*itor)->second->getType() == DIR_INODE)
                            dirents = directory_ptr_of(dirents.find(*itor)->second->getContents())->getmap();
              else
                     throw yshell_exn (*itor + " does NOT exist!");
              }
              directory_ptr_of(dirents["."]->getContents())->remove(*itor);
        }
   }
   else
        throw yshell_exn ("It cannot be removed");
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}
