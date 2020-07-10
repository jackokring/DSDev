#ifndef GAME__H
#define GAME__H
#include "ctl.h"
#include "gfx.h"
#include "sfx.h"
#include <nds.h>
#include <gl2d.h>
//============================== GAME INCLUDES ====================
extern u16 pRand(u16 range);
extern void qRand(u32 seed);

class GameLogic {
    public:
        //initialize
        void initGame();
        void startGame();
        //load/save
        void loadGame(int slot);
        void saveGame(int slot);
        //screen draw
        void drawMain();
        void drawSub();
        void draw3D();
        //game processing
        void processInputs(u16 keysMasked);
        void processMotions(int32 frames);//frames usually 1
        void processCollisions();
        void processStateMachine();
};

#endif