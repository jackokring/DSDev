#include "lang.h"
//========================== DSL PROCESSOR =================

u16 memory[65536];
char strings[65536];

int Link::asInt() {
    return 0;
}

bool Link::asBool() {
    return false;
}

char *Link::asString() {
    return NULL;
}

List *Link::asList() {
    return NULL;
}