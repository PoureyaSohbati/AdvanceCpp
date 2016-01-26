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

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

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

int main (int argc, char** argv) {
  // sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   string line, str1, str2;
   size_t found, lineNumber, choice;
   string filename = "-";
   str_str_map list;
   int flag = 0;

   if(argc == 1)
     flag = 1;

   for(int i = 1; i < argc || flag; ++i){
     lineNumber = 0;
     if(!flag)
         filename = argv[i];
     fstream file;
     fstream check(filename);
     if(!check && filename != "-")
           cerr << filename << ": Cannot be accessed." << endl;
     else{
     file.open(filename); 
     while(1){ 
       str1 = "";
       str2 = "";
       choice = 0; 
       ++lineNumber;
       if(filename != "-"){
            if(!getline(file, line))
                break;
            else{
                cout << filename << ": " << lineNumber << ": ";
                cout << line << endl;
            }
       }
       else {
            cout << filename << ": " << lineNumber << ": ";
            getline(cin, line);
            if(line == "exit")
               break;
       }
       found = line.find_first_not_of(" \t");
       if(found != string::npos and line.at(found) != '#'){
                  // if the line is not empty or is not a comment line
           if ((found = line.find_first_of("=")) != string::npos){ 
                  // if there is a '=' in the line
               choice += 2;
               str1 = line.substr(0, found);  
               str2 = line.substr(found+1);
               if((found = str1.find_first_not_of(" \t")) 
                       != string::npos){
                  // if the key is given in the line
                    choice += 1;
                    str1 = str1.substr(found);
                    str1 = str1.substr(0, 
                                str1.find_last_not_of(" \t")+1);
               }
               else  // if the key is not given in the line
                    str1 = "";
               if((found = str2.find_first_not_of(" \t"))
                       != string::npos){
                    // if the value is given in the line
                    choice += 3;
                    str2 = str2.substr(found);
                    str2 = str2.substr(0, 
                                str2.find_last_not_of(" \t")+1);
               }
               else  // if the value is not given in the line
                    str2 = "";
           }
           else{  
                 // if '=' is not given in the line
               choice += 1;
               str1 = line.substr(line.find_first_not_of(" \t"));
               str1 = str1.substr(0,
                            str1.find_last_not_of(" \t")+1);        
           }
        }
        switch (choice){
           // Empty or comment line
           case 0:
                    break;
           // key
           case 1:  {
                        str_str_map::iterator itor = list.find(str1);
                        if(itor == list.end()){
                            cout << str1 << ":key not found" << endl;
                        }
                        else{ 
                            cout << itor->first << " = ";
                            cout << itor->second << endl;
                        }
                        break;
                    }
           // =
           case 2:  for (str_str_map::iterator itor = list.begin();
                         itor != list.end(); ++itor){
                        cout << itor->first << " = ";
                        cout << itor->second << endl;
                    }
                    break;
           // key =
           case 3:  {
                        str_str_map::iterator itor = list.find(str1);
                        if(itor != list.end())
                             itor = list.erase(itor);
                        else
                             cout << str1 << ": key not found" << endl; 
                        break;
                    }
            // = value
            case 5:  for (str_str_map::iterator itor = list.begin();
                           itor != list.end(); ++itor){
                          if(itor->second == str2){
                             cout << itor->first << " = ";
                             cout << itor->second << endl;
                          }
                      }
                      break;
            // key = value
            case 6:  str_str_pair pair (str1, str2);
                     list.insert(pair);
                     cout << str1 << " = " << str2 << endl;
                     break;
        }//end of switch

     }//end of while
     if(flag)
        flag = 0;
     file.close();
   }//end else

   }//end of for

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}

