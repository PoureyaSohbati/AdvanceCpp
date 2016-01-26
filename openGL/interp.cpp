#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby" , &interpreter::do_moveby },
   {"border" , &interpreter::do_border },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle" , &interpreter::make_triangle  },
   {"right_triangle", &interpreter::make_right_triangle },
   {"isosceles", &interpreter::make_isosceles  },
   {"equilateral", &interpreter::make_equilateral  },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   rgbcolor color {begin[0]};
   //itor->second->draw (where, color);
   object temp(itor->second, where, color);
   window::push_back(temp);
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 1) throw runtime_error ("syntax error");
   window::setmoveby(stoi(begin[0]));
}


void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 2) throw runtime_error ("syntax error");
   rgbcolor color {begin[0]};
   window::setborder(color, from_string<GLfloat>(begin[1]));
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string temp = "";
   for(auto itor = begin+1; itor != end-1; ++itor){
       temp += *itor;
       if(itor != end-2)
          temp += " ";
   }
   cout << temp << endl << begin[0] << endl;
   return make_shared<text> (begin[0], temp);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<ellipse> (from_string<GLfloat>(begin[0]),
                                from_string<GLfloat>(begin[1]));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (from_string<GLfloat>(begin[0]));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list vertices;
   vertex temp;
   for(auto itor = begin; itor != end; ++itor){
        temp.xpos = from_string<GLfloat>(*itor);
        ++itor;
        temp.ypos = from_string<GLfloat>(*itor);
        vertices.push_back(temp);
   }
   return make_shared<polygon> (vertices);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<rectangle> (from_string<GLfloat>(begin[0]),
                                  from_string<GLfloat>(begin[1]));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (from_string<GLfloat>(begin[0]));
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<diamond> (from_string<GLfloat>(begin[0]),
                                from_string<GLfloat>(begin[1]));
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list vertices;
   vertex temp;
   for(auto itor = begin; itor != end; ++itor){
        temp.xpos = from_string<GLfloat>(*itor);
        ++itor;
        temp.ypos = from_string<GLfloat>(*itor);
        vertices.push_back(temp);
   }
   return make_shared<triangle> (vertices);
}

shape_ptr interpreter::make_right_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<right_triangle> (from_string<GLfloat>(begin[0]),
                                       from_string<GLfloat>(begin[1]));
}

shape_ptr interpreter::make_isosceles (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<isosceles> (from_string<GLfloat>(begin[0]),
                                  from_string<GLfloat>(begin[1]));
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<equilateral> (from_string<GLfloat>(begin[0]));
}
