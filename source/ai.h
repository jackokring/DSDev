#ifndef AI__H
#define AI__H
//====================== AI =============================
//state machine states
enum AIState {
    //coverage (binary polar)
    AIS_GROUP = 1,
    AIS_DISPERSE = 0,

    //aggression zero sum (binary polar)
    AIS_DESTROY = 2,
    AIS_LOVE = 0,

    //focus (binary polar)
    AIS_FOCUSED = 4,
    AIS_FICKLE = 0,

    //sense knowledge (binary polar)
    AIS_POSITION_LOCATED = 8,
    AIS_POSITION_ESTIMATED = 0,

    //predictive knowledge (binary polar, and inverse for DISPERSE)
    AIS_INTERCEPTS = 16,
    AIS_BLINE = 0,

    //newtonian motive (binary polar)
    AIS_CAN_IMPULSE = 32,
    AIS_FREE_FALL = 0,

    //can spawn (binary polar)
    AIS_IS_GENETIC = 64,
    AIS_IS_DRONE = 0,

    //can think (binary polar)
    AIS_IS_MIMETIC = 128,
    AIS_IS_MINDLESS = 0,

    //plasmid (binary polar)
    AIS_CAN_PLASMID = 256,
    AIS_FIXED_GENETIC = 0,

    //conversation (binary polar)
    AIS_CAN_LEARN = 512,
    AIS_FIXED_MIMETIC = 0,

    //polymorph (binary polar, i.e. can this enum be self-altered in life?)
    AIS_META_POLYMORPH = 1024,
    AIS_NOT_THE_THING = 0,

    //tooling up (binary polar, using weapon scores on the weapon net)
    AIS_CAN_UPGRADE = 2048,
    AIS_INTRINSIC_TOOLAGE = 0,

    //empathy (binary polar, local spreading of common loss functions)
    AIS_EMPATH = 4096,
    AIS_STEELY = 0,

    //weapon meta (binary polar, tree of distributed emissives)
    AIS_IS_WEAPON = 8192,
    AIS_IS_AMMOABLE = 0,

    //womb (binary polar)
    AIS_FEMALE = 16384,
    AIS_MALE = 0,

    //mouth (binary polar)
    AIS_IS_EATER = 32768,
    AIS_UNVORE = 0,

    //excretion (binary polar)
    AIS_SHITS = 65536,
    AIS_PERFECT_RECYCLER = 0,
}





#endif