//DSL processor
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

extern View *subViewRXInput;
extern uint keyIntercepted;