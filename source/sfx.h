#ifndef SFX__H
#define SFX__H
//============================== GRAPHICS INCLUDES ====================
#include <nds.h>
#include <maxmod9.h>
#include "mmsolution.h"

//cummulative totals for ? but may change ...
#define CMOD_WF_COURIER	(SFX_ZAP + 1)
#define CMOD_MATHHYSTERIA	(SFX_BIRD + 1)
#define CMOD_TIMETRAP	(CMOD_MATHHYSTERIA + 17)
#define CMOD_QUASAR	    (SFX_BOUNCE + 1)
#define CMOD_ALIENTEMPLE	(CMOD_QUASAR + 31)
#define CMOD_LILHOUSE	(CMOD_ALIENTEMPLE + 20)
#define CMOD_AGRESSOR8	(CMOD_LILHOUSE + 13)
#define CMOD_FLOOR_23	(SFX_BOOM + 1)
#define CMOD_TRICKTOP	(SFX_EXPLODE + 1)

//#define SFX_new CMOD_x + n

//ABXY? Not always
#define ACTION_FX SFX_ZAP
#define CANCEL_FX SFX_BOUNCE
#define INFO_FX SFX_BIRD
#define OPTION_FX SFX_UP

class Audio {
    public:
        static void playMod(u8 current);
        static mm_sfxhand playEffect(int effect, bool foreground = false);
        static void cueEffect(int effect, bool foreground = false);
};

#endif