#include "lang.h"
#include "ctl.h"
#include <string.h>
#include <keyboard.h>
//========================== DSL PROCESSOR =================
#define OUT_OF_MEMORY 0
#define STRING_TOO_LONG 1

//384 kB memory space
u16 memory[65536];
u16 links[65536];
char strings[65536];
u8 types[65536];
const u16 freeChain = 0;
u16 maxString;
u16 stringList;

void initMemory() {
    for(int i = 0; i < 65536; ++i) {
        links[i] = i + 1;//wraps modulo ring
    }
    //freeChain = 0;
    maxString = 0;
    memory[freeChain] = 65535;//free count
    stringList = 0;
}

void processError(u16 error) {
    //TODO:
}

bool memoryFull() {
    return (links[freeChain] == links[links[freeChain]]);//free chain points to self
    //last memory location not accessible. Not worth the index to access
}

bool stringFull(char *allocate) {
    return (maxString + strlen(allocate)) > 65535;//overflow with \0
    //a list of strings recycled can be kept to compact
}

u16 alloc() {
    if(!memoryFull()) {
        u16 loc = links[freeChain];
        u16 next = links[loc];
        links[freeChain] = next;
        links[loc] = 0;//empty tail
        memory[loc] = 0;//empty head
        memory[freeChain]--;//free count
        return loc;
    }
    processError(OUT_OF_MEMORY);
    return 0;
}

void free(u16 loc) {
    links[loc] = links[freeChain];
    links[freeChain] = loc;//add
    memory[freeChain]++;//free count
}

u16 allocString(char *allocate) {
    if(!stringFull(allocate)) {
        u16 loc = alloc();
        if(!loc) {
            return 0;//out of mem baulk
        }
        memory[loc] = maxString;//pointer
        while((strings[maxString++] = *(allocate++)));//copy string
        u16 handle = alloc();
        memory[handle] = loc;
        links[handle] = stringList;
        stringList = handle;
        return loc;
    }
    processError(STRING_TOO_LONG);
    return 0;
}

void freeString(u16 loc) {
    u16 start = memory[loc];
    u16 len = strlen(strings + start) + 1;//as null too
    u16 tmp = stringList;
    u16 *last = &stringList;
    while(tmp) {
        if(memory[memory[tmp]] > start) {//needs copy
            memory[memory[tmp]] -= len;//adjust pointer
        }
        if(memory[tmp] == loc) {//the string to delete
            u16 tmp2 = links[tmp];
            free(tmp);//free handle
            *last = tmp2;//delete handle
            free(loc);//free string pointer
            if(!*last) break;//final delete
        }
        last = &links[tmp];
        tmp = links[tmp];//next handle
    }
    for(u16 i = start + len; i < maxString; ++i) {
        strings[i - len] = strings[i];
    }
    maxString -= len;//reclaim space
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
    if(key == DVK_FOLD) keyboardVisible(false);//hide BUT has input finished?
}

void View::show(bool keyboard) {//might be virtual?
    //if keyboard true then 14 line display

}

bool View::VBIPoll() {
    if(baulkAI) return true;//been a while as VBI occured
    //obtaining a frame count of frames elapsed in main thread resets BaulkAI to false
    //process non essential threads here!!
    //or wait
    swiWaitForVBlank();//still some frame left to waste!!
    return true;
}
