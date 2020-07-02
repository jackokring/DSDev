#ifndef SFX__H
#define SFX__H
//============================== GRAPHICS INCLUDES ====================
#include <nds.h>
#include <maxmod9.h>
#include "mmsolution.h"

//cummulative totals for ? but may change ...
//#define SFX_DRONE_C4	0
#define CMOD_WF_COURIER	1
//#define SFX_ZAP	32
//#define SFX_CHIPPULSE_C4	33
//#define SFX_GRISTLE_C4	34
#define CMOD_MATHHYSTERIA	35
#define CMOD_TIMETRAP	(CMOD_MATHHYSTERIA + 17)
#define CMOD_QUASAR	    (CMOD_TIMETRAP + 17)
#define CMOD_ALIENTEMPLE	(CMOD_QUASAR + 31)
#define CMOD_LILHOUSE	(CMOD_ALIENTEMPLE + 20)
//#define SFX_POWERX_C2	190
#define CMOD_AGRESSOR8	191
//#define SFX_CRISPERROR_C4	222
//#define SFX_ALERTZONE_C3	223
#define CMOD_FLOOR_23	224
//#define SFX_ELECTROGLICK_C2	255
//#define SFX_EXPLODE	256
#define CMOD_TRICKTOP	257
//#define SFX_ROBOZ_C4	288

//#define SFX_new CMOD_x + n

//ABXY? Not always
#define ACTION_FX SFX_ZAP
#define CANCEL_FX SFX_CRISPERROR_C4
#define INFO_FX SFX_CHIPPULSE_C4
#define OPTION_FX SFX_DRONE_C4

class Audio {
    public:
        static void playMod(u8 current);
        static mm_sfxhand playEffect(int effect, bool foreground = false);
};

#endif