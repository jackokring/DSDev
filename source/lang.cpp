#include "lang.h"
#include <string.h>
//========================== DSL PROCESSOR =================

//384 kB memory space
u16 memory[65536];
u16 links[65536];
char strings[65536];
u8 types[65536];
u16 freeChain;
u16 maxString;

void initMemory() {
    for(int i = 0; i < 65536; ++i) {
        links[i] = i + 1;//wraps modulo ring
    }
    freeChain = 0;
    maxString = 0;
}

bool memoryFull() {
    return (links[freeChain] == links[links[freeChain]]);//free chain points to self
    //last memory location not accessible. Not worth the index to access
}

bool stringFull(char *alloc) {
    return (maxString + strlen(alloc)) > 65535;//overflow with \0
    //a list of strings recycled can be kept to compact
} 

int asInt(u16 node) {
    return memory[node];
}

bool asBool(u16 node) {
    return memory[node] != 0;
}

char *asString(u16 node) {
    return strings + memory[node];
}

void View::processInput(u16 keys) {
	
}

void View::keyboardPress(int key) {

}

void View::show() {//might be virtual?

}
