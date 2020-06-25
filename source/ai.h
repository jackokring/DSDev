#ifndef AI__H
#define AI__H
//============================== AI HANDLER AND GENERAL INCLUDES ====================
#include <nds.h>
extern bool baulkAI;
extern int32 frame;
extern int32 stepFrames;
extern bool paused;
extern bool inGame;
extern bool exiting;
extern bool newGame;

enum CORNER {
    TOP_LEFT_FONT = 0,
    TOP_RIGHT_FONT = 1,
    BOTTOM_LEFT_FONT = 2,
    BOTTOM_RIGHT_FONT = 3
};

#define RED(X) ((X >> 0) & 31)
#define GREEN(X) ((X >> 5) & 31)
#define BLUE(X) ((X >> 10)& 31)
#define KEY_A_OR_START (KEY_A | KEY_START)
#define KEY_ALL_BUTTONS (KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R | KEY_START | KEY_SELECT)
#define KEY_DPAD (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)
#endif