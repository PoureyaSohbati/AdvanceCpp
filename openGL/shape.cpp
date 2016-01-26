#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"
#include "graphics.h"
#include <cmath>

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (const string& font, const string& text){
   DEBUGF ('c', this);
   textdata = text;
   auto fontptr = fontcode.find(font); 
   glut_bitmap_font = fontptr->second;
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);  
}

triangle::triangle (const vertex_list& vertices) : polygon(vertices) {
   DEBUGF ('c', this);
}

right_triangle::right_triangle (GLfloat width, GLfloat height):
                  triangle({}) {
   vertex temp;
   temp.xpos = 0;
   temp.ypos = 0;
   vertices.push_back(temp); 
   temp.xpos = 0;
   temp.ypos = height;
   vertices.push_back(temp);
   temp.xpos = width;
   temp.ypos = 0;
   vertices.push_back(temp);
}

isosceles::isosceles (GLfloat width, GLfloat height):
             triangle({}) {
   vertex temp;
   temp.xpos = 0;
   temp.ypos = height;
   vertices.push_back(temp);
   temp.xpos = -1 * (width/2);
   temp.ypos = 0;
   vertices.push_back(temp);
   temp.xpos = width/2;
   temp.ypos = 0;
   vertices.push_back(temp);
}

equilateral::equilateral (GLfloat width) : isosceles(width, width) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex temp;
   temp.xpos = -1 * (width/2);
   temp.ypos = height/2;
   vertices.push_back(temp);
   temp.xpos = width/2;
   temp.ypos = height/2;
   vertices.push_back(temp);
   temp.xpos = width/2;
   temp.ypos = -1 * (height/2);
   vertices.push_back(temp);
   temp.xpos = -1 * (width/2);
   temp.ypos = -1 * (height/2);
   vertices.push_back(temp);
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height):
            polygon({}) { 
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex temp;
   temp.xpos = 0;
   temp.ypos = -1 * (height/2);
   vertices.push_back(temp);
   temp.xpos = width/2;
   temp.ypos = 0;
   vertices.push_back(temp);
   temp.xpos = 0;
   temp.ypos = height/2;
   vertices.push_back(temp);
   temp.xpos = -1 * (width/2);
   temp.ypos = 0;
   vertices.push_back(temp);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   auto text = reinterpret_cast<const GLubyte*> (textdata.c_str());
   glColor3ub(color.red, color.green, color.blue); 
   //size_t width = glutBitmapLength (glut_bitmap_font, text);
   //size_t height = glutBitmapHeight (glut_bitmap_font);
   float xpos = center.xpos; // - (width/2);
   float ypos = center.ypos; // - (height/2);
   glRasterPos2f (xpos, ypos);
   glutBitmapString (glut_bitmap_font, text);
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ub(color.red, color.green, color.blue);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = dimension.xpos * cos (theta) + center.xpos;
      float ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
   if(window::haveborder()){
      glBegin(GL_LINE_LOOP);
      rgbcolor borderC = window::getborderColor();
      glColor3ub(borderC.red, borderC.green, borderC.blue);
      for (float theta = 0; theta < 2 * M_PI; theta += delta) {
         float xpos = dimension.xpos * cos (theta) + center.xpos;
         float ypos = dimension.ypos * sin (theta) + center.ypos;
         glVertex2f (xpos, ypos);
      }
      glEnd(); 
   }
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glColor3ub(color.red, color.green, color.blue);
   for(size_t i = 0; i < vertices.size(); ++i)
       glVertex2f(vertices[i].xpos+center.xpos,
                  vertices[i].ypos+center.ypos);
   glEnd();
   if(window::haveborder()){
       glBegin(GL_LINE_LOOP);
       rgbcolor borderC = window::getborderColor();
       glColor3ub(borderC.red, borderC.green, borderC.blue);
       for(size_t i = 0; i < vertices.size(); ++i)
           glVertex2f(vertices[i].xpos+center.xpos, 
                      vertices[i].ypos+center.ypos);
       glEnd();
   }
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

