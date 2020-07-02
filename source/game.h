#ifndef GAME__H
#define GAME__H
#include "ctl.h"
#include "gfx.h"
#include "sfx.h"
#include <nds.h>
#include <gl2d.h>
//============================== GAME INCLUDES ====================

class GameLogic {
    public:
        void drawMain();
        void drawSub();
        void draw3D();
        void processInputs(u16 keysMasked);
        void processMotions();
        void processCollisions();
        void processStateMachine();
};

#endif