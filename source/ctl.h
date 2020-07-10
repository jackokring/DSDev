#ifndef CTL__H
#define CTL__H
//============================== GENERAL INCLUDES ====================
#include <nds.h>
extern bool baulkAI;
extern int32 frame;
//extern int32 stepFrames;
extern bool paused;
extern u16 keyIntercepted;
extern u16 keyHoldAllow;

enum CORNER {
    TOP_LEFT_FONT = 0,
    TOP_RIGHT_FONT = 1,
    BOTTOM_LEFT_FONT = 2,
    BOTTOM_RIGHT_FONT = 3
};

enum FORMAT_PRINT {
    PERCENT,
    ON_OFF
};

#define RED(X) ((X >> 0) & 31)
#define GREEN(X) ((X >> 5) & 31)
#define BLUE(X) ((X >> 10) & 31)
#define KEY_A_OR_START (KEY_A | KEY_START)
#define KEY_ALL_BUTTONS (KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R | KEY_START | KEY_SELECT)
#define KEY_DPAD_X (KEY_LEFT | KEY_RIGHT)
#define KEY_DPAD_Y (KEY_UP | KEY_DOWN)
#define KEY_DPAD (KEY_DPAD_Y | KEY_DPAD_X)

#define ANSI_RED "\e[1m\e[41m"
#define ANSI_YEL "\e[1m\e[43m"
#define ANSI_BLU "\e[1m\e[44m"
#define ANSI_GRN "\e[1m\e[42m"
#define ANSI_WHT "\e[1m\e[47m"

class CTL {
    public:
        static void gameLostResume();
        static void gameCompleteRestart();
};
#endif