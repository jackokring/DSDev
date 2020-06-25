#include <nds.h>
#include "mmsolution.h"
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