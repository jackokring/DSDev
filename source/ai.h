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

    //predictive knowledge (binary polar)
    AIS_INTERCEPTS = 16,
    AIS_BLINE = 0,

    //newtonian motive (binary polar)
    AIS_CAN_IMPULSE = 32,
    AIS_FREE_FALL = 0,

    
}





#endif