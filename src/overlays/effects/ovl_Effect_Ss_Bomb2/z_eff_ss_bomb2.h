#ifndef _Z64_EFFECT_SS_BOMB2_H_
#define _Z64_EFFECT_SS_BOMB2_H_

#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x26 */ s16 scaleStep;
    /* 0x28 */ u8 drawMode;
} EffectSsBomb2InitParams; // size = 0x30

extern const EffectSsInit Effect_Ss_Bomb2_InitVars;

#endif
