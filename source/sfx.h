#include <nds.h>
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

u16 audioEffects[] = {
    //primary feedback sound set
    SFX_ZAP,//primary action feedback
    SFX_CRISPERROR_C4,//displeasure or cancel sound
    SFX_CHIPPULSE_C4,//working on it sound, tick, tick ..
    SFX_DRONE_C4,//buzzing about somewhere

    //must have game sounds
	SFX_EXPLODE,//classic bang reverb

	SFX_ALERTZONE_C3,//bubbly query
    SFX_ELECTROGLICK_C2,//boing electro thing
    SFX_GRISTLE_C4,//distorted chopper
    SFX_POWERX_C2,//electro buzz stab
    SFX_ROBOZ_C4//vocoder-ish
};

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