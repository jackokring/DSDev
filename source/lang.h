//DSL processor
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
extern uint keyIntercepted;//mask of keys intercepted