#include "lang.h"
//========================== DSL PROCESSOR =================

u16 memory[65536];
char strings[65536];
u8 types[65536];

int asInt(u16 node) {
    return memory[node];
}

bool asBool(u16 node) {
    return memory[node] != 0;
}

char *asString(u16 node) {
    return strings + memory[node];
}
