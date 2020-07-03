#ifndef LANG__H
#define LANG__H
//========================== DSL PROCESSOR =================
#include <nds.h>
#include <stdio.h>

extern u16 memory[65536];//can be used as a loading buffer
extern char strings[65536];

class List;

class Link {
    u16 val;
    u16 next;
    int asInt();
    bool asBool();
    char *asString();
    List *asList();
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
