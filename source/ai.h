#ifndef AI__H
#define AI__H
//============================== AI HANDLER AND GENERAL INCLUDES ====================
#include <nds.h>
extern bool baulkAI;
extern int32 frame;
extern bool paused;
extern bool inGame;
extern bool exiting;

enum CORNER {
    TOP_LEFT_FONT = 0,
    TOP_RIGHT_FONT = 1,
    BOTTOM_LEFT_FONT = 2,
    BOTTOM_RIGHT_FONT = 3
};

#define RED(X) ((X >> 0) & 31)
#define GREEN(X) ((X >> 5) & 31)
#define BLUE(X) ((X >> 10)& 31)
#endif