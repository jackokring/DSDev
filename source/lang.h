#ifndef LANG__H
#define LANG__H
//========================== DSL PROCESSOR =================
#include <nds.h>
#include <stdio.h>

class Type;

class Link {
    Type *val;
    Link *next;
};

class Type : Link {

};

class Number : Type {

};

class String : Type {

};

class List : Type {

};

class Symbol : Type {

};

class Protected : Type {

};

class EditPoint : Type {//symbol and inner pointer

};

class Book : Type {

};

class Macro : Symbol {//immediate

};

//thread
class Thread : Type {//cooperative

};

//views
class View : Type {

};

class ViewSet : View {

};

//machine
class Machine {//singleton?

};

extern View *subViewRXInput;//set view to draw?
extern u16 keyIntercepted;//mask of keys intercepted
#endif
