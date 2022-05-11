/*
 * File: z_en_raf.c
 * Overlay: ovl_En_Raf
 * Description: Carnivorous Lily Pad
 */

#include "z_en_raf.h"

#define FLAGS (ACTOR_FLAG_8000000)

#define THIS ((EnRaf*)thisx)

void EnRaf_Init(Actor* thisx, GlobalContext* globalCtx);
void EnRaf_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnRaf_Update(Actor* thisx, GlobalContext* globalCtx);
void EnRaf_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnRaf_SetupIdle(EnRaf* this);
void EnRaf_Idle(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_SetupGrab(EnRaf* this);
void EnRaf_Grab(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_SetupChew(EnRaf* this);
void EnRaf_Chew(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_SetupThrow(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_Throw(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_Explode(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_PostDetonation(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_Convulse(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_SetupDissolve(EnRaf* this);
void EnRaf_Dissolve(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_SetupDormant(EnRaf* this);
void EnRaf_Dormant(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_InitializeParticle(EnRaf* this, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale, s16 timer);
void EnRaf_UpdateParticles(EnRaf* this, GlobalContext* globalCtx);
void EnRaf_DrawParticles(EnRaf* this, GlobalContext* globalCtx);

typedef enum {
    /* 0 */ EN_RAF_ANIMATION_IDLE,
    /* 1 */ EN_RAF_ANIMATION_CLOSE,
    /* 2 */ EN_RAF_ANIMATION_CHEW,
    /* 3 */ EN_RAF_ANIMATION_SPIT,
    /* 4 */ EN_RAF_ANIMATION_CONVULSE,
    /* 5 */ EN_RAF_ANIMATION_DEATH,
} EnRafAnimationIndex;

typedef enum {
    /* 0 */ EN_RAF_ACTION_IDLE,
    /* 1 */ EN_RAF_ACTION_GRAB,
    /* 2 */ EN_RAF_ACTION_CHEW,
    /* 3 */ EN_RAF_ACTION_THROW,
    /* 4 */ EN_RAF_ACTION_EXPLODE,
    /* 5 */ EN_RAF_ACTION_CONVULSE,
    /* 6 */ EN_RAF_ACTION_DISSOLVE,
    /* 7 */ EN_RAF_ACTION_DORMANT,
} EnRafAction;

typedef enum {
    /* 0 */ EN_RAF_GRAB_TARGET_PLAYER,
    /* 1 */ EN_RAF_GRAB_TARGET_EXPLOSIVE,
    /* 2 */ EN_RAF_GRAB_TARGET_GORON_PLAYER
} EnRafGrabTarget;

typedef enum {
    /* 0 */ EN_RAF_PETAL_SCALE_TYPE_DEAD,
    /* 1 */ EN_RAF_PETAL_SCALE_TYPE_GRAB,
    /* 2 */ EN_RAF_PETAL_SCALE_TYPE_CHEW,
    /* 3 */ EN_RAF_PETAL_SCALE_TYPE_IDLE_OR_THROW
} EnRafPetalScaleType;

const ActorInit En_Raf_InitVars = {
    ACTOR_EN_RAF,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_RAF,
    sizeof(EnRaf),
    (ActorFunc)EnRaf_Init,
    (ActorFunc)EnRaf_Destroy,
    (ActorFunc)EnRaf_Update,
    (ActorFunc)EnRaf_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 50, 10, -10, { 0, 0, 0 } },
};

static u8 sTeethClearPixelTableFirstPass[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
    1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0,
};

static u8 sTeethClearPixelTableSecondPass[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static u8 sPetalClearPixelTableFirstPass[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0,
    1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1,
    1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
    0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0,
    0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0,
    0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1,
};

static u8 sPetalClearPixelTableSecondPass[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0xF),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xE),
    /* Zora boomerang */ DMG_ENTRY(0, 0xF),
    /* Normal arrow   */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(0, 0xF),
    /* Sword          */ DMG_ENTRY(0, 0xF),
    /* Goron pound    */ DMG_ENTRY(0, 0xF),
    /* Fire arrow     */ DMG_ENTRY(0, 0xF),
    /* Ice arrow      */ DMG_ENTRY(0, 0xF),
    /* Light arrow    */ DMG_ENTRY(0, 0xF),
    /* Goron spikes   */ DMG_ENTRY(0, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0xF),
    /* Deku bubble    */ DMG_ENTRY(0, 0xF),
    /* Deku launch    */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0xF),
    /* Zora barrier   */ DMG_ENTRY(0, 0xF),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0xF),
    /* Zora punch     */ DMG_ENTRY(0, 0xF),
    /* Spin attack    */ DMG_ENTRY(0, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0xF),
};

/**
 * Sets the `index`th pixel of the trap teeth texture to 0 (transparent black)
 * according to the `clearPixelTable`. Only works if the texture uses 16-bit pixels.
 */
void EnRaf_ClearPixelTeeth(u16* texture, u8* clearPixelTable, s32 index) {
    if ((index < (8 * 8)) && (clearPixelTable[index] != 0)) {
        texture[index] = 0;
    }
}

/**
 * Sets the `index`th pixel of the trap petal texture to 0 (transparent black)
 * according to the `clearPixelTable`. Only works if the texture uses 16-bit pixels.
 */
void EnRaf_ClearPixelPetal(u16* texture, u8* clearPixelTable, s32 index) {
    if (clearPixelTable[index] != 0) {
        texture[index] = 0;
    }
}

void EnRaf_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnRaf* this = THIS;
    Vec3f limbScale = { 1.0f, 1.0f, 1.0f };
    s32 pad;
    s32 i;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gCarnivorousLilyPadCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    Collider_InitAndSetCylinder(globalCtx, &this->collider, &this->dyna.actor, &sCylinderInit);
    this->dyna.actor.targetMode = 3;
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gCarnivorousLilyPadSkel, &gCarnivorousLilyPadSpitAnim,
                       this->jointTable, this->morphTable, CARNIVOROUS_LILY_PAD_LIMB_MAX);

    for (i = 0; i < ARRAY_COUNT(this->limbScale); i++) {
        Math_Vec3f_Copy(&this->targetLimbScale[i], &limbScale);
        Math_Vec3f_Copy(&this->limbScale[i], &limbScale);
    }

    this->dyna.actor.colChkInfo.damageTable = &sDamageTable;
    this->dyna.actor.colChkInfo.health = BREG(1) + 2;
    this->mainType = EN_RAF_GET_TYPE(&this->dyna.actor);
    this->reviveTimer = EN_RAF_GET_REVIVE_TIMER(&this->dyna.actor);
    this->switchFlag = EN_RAF_GET_SWITCH_FLAG(&this->dyna.actor);
    if (this->switchFlag == 0x7F) {
        this->switchFlag = -1;
    }

    if (this->reviveTimer == 31) {
        this->reviveTimer = -1;
    } else {
        this->reviveTimer = 30;
    }

    if (((this->switchFlag >= 0) || (this->mainType == EN_RAF_TYPE_DORMANT) ||
         (gSaveContext.save.weekEventReg[12] & 1)) &&
        ((Flags_GetSwitch(globalCtx, this->switchFlag)) || (this->mainType == EN_RAF_TYPE_DORMANT))) {
        s32 i;

        for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT; i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT;
             i++) {
            Math_Vec3f_Copy(&this->limbScale[i], &gZeroVec3f);
            Math_Vec3f_Copy(&this->targetLimbScale[i], &gZeroVec3f);
        }

        EnRaf_SetupDormant(this);
    } else {
        this->bobPhase = Rand_ZeroFloat(1.0f) * 20000.0f;
        Actor_SetScale(&this->dyna.actor, 0.01f);
        EnRaf_SetupIdle(this);
    }
}

void EnRaf_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnRaf* this = THIS;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void EnRaf_ChangeAnimation(EnRaf* this, s32 index) {
    static AnimationHeader* sAnimations[] = {
        &gCarnivorousLilyPadSpitAnim, &gCarnivorousLilyPadCloseAnim,    &gCarnivorousLilyPadChewAnim,
        &gCarnivorousLilyPadSpitAnim, &gCarnivorousLilyPadConvulseAnim, &gCarnivorousLilyPadDeathAnim,
    };
    static u8 sAnimationModes[] = {
        ANIMMODE_ONCE, ANIMMODE_ONCE, ANIMMODE_LOOP, ANIMMODE_ONCE, ANIMMODE_LOOP, ANIMMODE_ONCE,
    };
    f32 startFrame = 0.0f;
    f32 playSpeed = 1.0f;

    this->endFrame = Animation_GetLastFrame(sAnimations[index]);
    if (index == EN_RAF_ANIMATION_IDLE) {
        startFrame = this->endFrame;
    } else if (index == EN_RAF_ANIMATION_CLOSE) {
        playSpeed = 2.0f;
    }

    Animation_Change(&this->skelAnime, sAnimations[index], playSpeed, startFrame, this->endFrame,
                     sAnimationModes[index], -4.0f);
}

void EnRaf_SetupIdle(EnRaf* this) {
    Vec3f targetLimbScale = { 1.0f, 1.0f, 1.0f };
    s32 i;

    EnRaf_ChangeAnimation(this, EN_RAF_ANIMATION_IDLE);

    for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT; i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT; i++) {
        Math_Vec3f_Copy(&this->targetLimbScale[i], &targetLimbScale);
    }

    this->petalScaleType = EN_RAF_PETAL_SCALE_TYPE_IDLE_OR_THROW;
    this->action = EN_RAF_ACTION_IDLE;
    this->actionFunc = EnRaf_Idle;
}

/**
 * Sits around waiting for the player or an explosive to get near in order to grab them.
 */
void EnRaf_Idle(EnRaf* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* explosive;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;

    if (this->timer == 0) {
        if ((player->transformation != PLAYER_FORM_DEKU) &&
            (this->dyna.actor.xzDistToPlayer < (BREG(48) + 80.0f) && (player->invincibilityTimer == 0) &&
             DynaPolyActor_IsInRidingMovingState(&this->dyna) && !(player->stateFlags1 & 0x8000000) &&
             globalCtx->grabPlayer(globalCtx, player))) {
            player->actor.parent = &this->dyna.actor;
            this->grabTarget = EN_RAF_GRAB_TARGET_PLAYER;

            if (player->transformation == PLAYER_FORM_GORON) {
                this->grabTarget = EN_RAF_GRAB_TARGET_GORON_PLAYER;
            } else {
                player->unk_AE8 = 50;
            }

            this->playerRotYWhenGrabbed = player->actor.world.rot.y;
            EnRaf_SetupGrab(this);
            return;
        }

        if ((globalCtx->gameplayFrames % 2) == 0) {
            return;
        }

        explosive = globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
        while (explosive != NULL) {
            // This check is pointless, since EnRaf is never in the explosive category.
            if ((EnRaf*)explosive == this) {
                explosive = explosive->next;
                continue;
            }

            xDiff = explosive->world.pos.x - this->dyna.actor.world.pos.x;
            yDiff = explosive->world.pos.y - this->dyna.actor.world.pos.y;
            zDiff = explosive->world.pos.z - this->dyna.actor.world.pos.z;
            if ((fabsf(xDiff) < 80.0f) && (fabsf(yDiff) < 30.0f) && (fabsf(zDiff) < 80.0f) &&
                (explosive->update != NULL) && (explosive->velocity.y != 0.0f)) {
                Actor_MarkForDeath(explosive);
                this->grabTarget = EN_RAF_GRAB_TARGET_EXPLOSIVE;
                this->collider.dim.radius = 30;
                this->collider.dim.height = 90;
                this->collider.dim.yShift = -10;
                EnRaf_SetupGrab(this);
                return;
            }

            explosive = explosive->next;
        }
    }
}

void EnRaf_SetupGrab(EnRaf* this) {
    EnRaf_ChangeAnimation(this, EN_RAF_ANIMATION_CLOSE);
    this->petalScaleType = EN_RAF_PETAL_SCALE_TYPE_GRAB;
    Actor_PlaySfxAtPos(&this->dyna.actor, NA_SE_EN_SUISEN_DRINK);
    this->action = EN_RAF_ACTION_GRAB;
    this->actionFunc = EnRaf_Grab;
}

/**
 * Grabs the player or explosive that entered its range.
 */
void EnRaf_Grab(EnRaf* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->grabTarget != EN_RAF_GRAB_TARGET_EXPLOSIVE) && (player->stateFlags2 & 0x80) &&
        (&this->dyna.actor == player->actor.parent)) {
        Math_ApproachF(&player->actor.world.pos.x, this->dyna.actor.world.pos.x, 0.3f, 10.0f);
        Math_ApproachF(&player->actor.world.pos.y, this->dyna.actor.world.pos.y, 0.3f, 10.0f);
        Math_ApproachF(&player->actor.world.pos.z, this->dyna.actor.world.pos.z, 0.3f, 10.0f);
    }

    if (this->endFrame <= curFrame) {
        EnRaf_SetupChew(this);
    }
}

void EnRaf_SetupChew(EnRaf* this) {
    s32 i;

    EnRaf_ChangeAnimation(this, EN_RAF_ANIMATION_CHEW);
    this->chewCount = 0;
    for (i = 0; i < ARRAY_COUNT(this->chewLimbRot); i++) {
        this->chewLimbRot[i].x = Rand_S16Offset(8, 8) << 8;
        this->chewLimbRot[i].y = Rand_S16Offset(8, 8) << 8;
        this->chewLimbRot[i].z = Rand_S16Offset(8, 8) << 8;
    }

    this->petalScaleType = EN_RAF_PETAL_SCALE_TYPE_CHEW;
    this->action = EN_RAF_ACTION_CHEW;
    this->actionFunc = EnRaf_Chew;
}

/**
 * Chews the player or explosive that was grabbed. If it grabbed a non-Goron player, it will deal
 * damage to them and eventually throw them. If it grabbed a Goron player or explosive, it will
 * instead explode after chewing them a bit.
 */
void EnRaf_Chew(EnRaf* this, GlobalContext* globalCtx) {
    f32 targetChewScale;
    f32 curFrame;
    Player* player = GET_PLAYER(globalCtx);

    curFrame = this->skelAnime.curFrame;
    targetChewScale = (BREG(51) / 100.0f) + 0.2f;
    Math_ApproachF(&this->chewScale, targetChewScale, 0.2f, 0.03f);

    if ((player->stateFlags2 & 0x80) && (this->grabTarget != EN_RAF_GRAB_TARGET_EXPLOSIVE) &&
        (&this->dyna.actor == player->actor.parent)) {
        Math_ApproachF(&player->actor.world.pos.x, this->dyna.actor.world.pos.x, 0.3f, 10.0f);
        Math_ApproachF(&player->actor.world.pos.y, this->dyna.actor.world.pos.y, 0.3f, 10.0f);
        Math_ApproachF(&player->actor.world.pos.z, this->dyna.actor.world.pos.z, 0.3f, 10.0f);
    }

    if (this->endFrame <= curFrame) {
        if (BREG(52) == 0) {
            this->chewCount++;
        }

        Actor_PlaySfxAtPos(&this->dyna.actor, NA_SE_EN_SUISEN_EAT);
        switch (this->grabTarget) {
            case EN_RAF_GRAB_TARGET_PLAYER:
                globalCtx->damagePlayer(globalCtx, -2);
                func_800B8E58((Player*)this, player->ageProperties->unk_92 + NA_SE_VO_LI_DAMAGE_S);
                CollisionCheck_GreenBlood(globalCtx, NULL, &player->actor.world.pos);
                if ((this->chewCount > (BREG(53) + 5)) || !(player->stateFlags2 & 0x80)) {
                    player->actor.freezeTimer = 10;
                    EnRaf_SetupThrow(this, globalCtx);
                    return;
                }
                break;

            case EN_RAF_GRAB_TARGET_EXPLOSIVE:
                Actor_ApplyDamage(&this->dyna.actor);
                if (this->chewCount > (BREG(54) + 4)) {
                    EnRaf_Explode(this, globalCtx);
                    return;
                }
                break;

            case EN_RAF_GRAB_TARGET_GORON_PLAYER:
                if (this->chewCount > (BREG(54) + 4)) {
                    player->actor.parent = NULL;
                    player->unk_AE8 = 1000;
                    EnRaf_Explode(this, globalCtx);
                }
                break;
        }
    }
}

void EnRaf_SetupThrow(EnRaf* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    EnRaf_ChangeAnimation(this, EN_RAF_ANIMATION_SPIT);
    player->actor.freezeTimer = 10;
    this->petalScaleType = EN_RAF_PETAL_SCALE_TYPE_IDLE_OR_THROW;
    this->action = EN_RAF_ACTION_THROW;
    this->actionFunc = EnRaf_Throw;
}

/**
 * Spits out the grabbed player.
 */
void EnRaf_Throw(EnRaf* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 curFrame = this->skelAnime.curFrame;

    if (Animation_OnFrame(&this->skelAnime, 10.0f)) {
        player->actor.freezeTimer = 0;
        player->actor.parent = NULL;
        Actor_PlaySfxAtPos(&this->dyna.actor, NA_SE_EN_SUISEN_THROW);
        func_800B8D50(globalCtx, &this->dyna.actor, BREG(55) + 3.0f, this->playerRotYWhenGrabbed + 0x8000,
                      BREG(56) + 10.0f, 0);
    } else if (curFrame < 10.0f) {
        player->actor.freezeTimer = 10;
    }

    if (this->endFrame <= curFrame) {
        this->petalScaleType = EN_RAF_PETAL_SCALE_TYPE_IDLE_OR_THROW;
        this->action = EN_RAF_ACTION_IDLE;
        this->timer = 20;
        this->actionFunc = EnRaf_Idle;
    }
}

/**
 * Creates an explosion effect/sound and spawns particles.
 */
void EnRaf_Explode(EnRaf* this, GlobalContext* globalCtx) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f acceleration = { 0.0f, 0.0f, 0.0f };
    Vec3f explosionPos;
    s32 i;
    s32 pad;

    this->action = EN_RAF_ACTION_EXPLODE;
    Math_Vec3f_Copy(&explosionPos, &this->dyna.actor.world.pos);
    explosionPos.y += 10.0f;
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_CLEAR_TAG, explosionPos.x, explosionPos.y, explosionPos.z, 0,
                0, 0, CLEAR_TAG_SMALL_EXPLOSION);
    Actor_PlaySfxAtPos(&this->dyna.actor, NA_SE_IT_BOMB_EXPLOSION);
    Actor_PlaySfxAtPos(&this->dyna.actor, NA_SE_EN_SUISEN_DEAD);
    if (this->switchFlag >= 0) {
        Flags_SetSwitch(globalCtx, this->switchFlag);
    }

    this->petalScaleType = EN_RAF_PETAL_SCALE_TYPE_DEAD;
    for (i = 0; i < BREG(57) + 30; i++) {
        acceleration.x = (Rand_ZeroOne() - 0.5f) * 0.5f;
        acceleration.y = -0.3f;
        acceleration.z = (Rand_ZeroOne() - 0.5f) * 0.5f;
        velocity.x = Rand_ZeroOne() - 0.5f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = Rand_ZeroOne() - 0.5f;
        EnRaf_InitializeParticle(this, &this->dyna.actor.world.pos, &velocity, &acceleration,
                                 (Rand_ZeroFloat(1.0f) / 500.0f) + 0.002f, 90);
    }

    for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT; i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT; i++) {
        Math_Vec3f_Copy(&this->targetLimbScale[i], &gZeroVec3f);
    }

    this->timer = 5;
    if (this->grabTarget == EN_RAF_GRAB_TARGET_EXPLOSIVE) {
        func_800BC154(globalCtx, &globalCtx->actorCtx, &this->dyna.actor, 5);
        this->dyna.actor.flags |= (ACTOR_FLAG_1 | ACTOR_FLAG_4);
    }

    this->actionFunc = EnRaf_PostDetonation;
}

/**
 * Switches the lily pad to the "dead" state once the timer reaches 0.
 */
void EnRaf_PostDetonation(EnRaf* this, GlobalContext* globalCtx) {
    if (this->timer == 0) {
        this->collider.dim.radius = 50;
        this->collider.dim.height = 10;
        func_800BC154(globalCtx, &globalCtx->actorCtx, &this->dyna.actor, 6);
        this->dyna.actor.flags &= ~(ACTOR_FLAG_1 | ACTOR_FLAG_4);
        EnRaf_SetupDormant(this);
    } else if (this->grabTarget == EN_RAF_GRAB_TARGET_EXPLOSIVE) {
        this->collider.dim.radius = 80;
        this->collider.dim.height = 50;
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }
}

void EnRaf_SetupConvulse(EnRaf* this) {
    EnRaf_ChangeAnimation(this, EN_RAF_ANIMATION_CONVULSE);
    this->chewCount = 0;
    this->action = EN_RAF_ACTION_CONVULSE;
    this->actionFunc = EnRaf_Convulse;
}

/**
 * Plays the convulsing animation and sets the lily pad's switch flag to prevent it from
 * ever coming back to life. When the water in Woodfall Temple is purified, this function
 * and EnRaf_Dissolve are jointly responsible for controlling the lily pad's death.
 */
void EnRaf_Convulse(EnRaf* this, GlobalContext* globalCtx) {
    f32 curFrame = this->skelAnime.curFrame;

    if (this->endFrame <= curFrame) {
        this->chewCount++;
        if (this->chewCount > (BREG(2) + 2)) {
            if (this->switchFlag >= 0) {
                Flags_SetSwitch(globalCtx, this->switchFlag);
            }

            EnRaf_SetupDissolve(this);
        }
    }
}

void EnRaf_SetupDissolve(EnRaf* this) {
    EnRaf_ChangeAnimation(this, EN_RAF_ANIMATION_DEATH);
    this->action = EN_RAF_ACTION_DISSOLVE;
    this->dissolveTimer = 0;
    this->actionFunc = EnRaf_Dissolve;
}

/**
 * Makes the trap petals on the lily pad dissolve and switches the lily pad to the "dead" state.
 * When the water in Woodfall Temple is purified, this function and EnRaf_Convulse are jointly
 * responsible for controlling the lily pad's death.
 */
void EnRaf_Dissolve(EnRaf* this, GlobalContext* globalCtx) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 i;

    if (this->endFrame <= curFrame) {
        this->dissolveTimer++;
        if (this->dissolveTimer < (BREG(3) + 105)) {
            for (i = 0; i < (BREG(4) + 5); i++) {
                EnRaf_ClearPixelPetal(Lib_SegmentedToVirtual(&gCarnivorousLilyPadTrapPetalTex),
                                      sPetalClearPixelTableFirstPass, this->petalClearPixelFirstPassIndex);
                EnRaf_ClearPixelTeeth(Lib_SegmentedToVirtual(&gCarnivorousLilyPadTrapTeethTex),
                                      sTeethClearPixelTableFirstPass, this->teethClearPixelFirstPassIndex);
                if (this->petalClearPixelFirstPassIndex < (16 * 32)) {
                    this->petalClearPixelFirstPassIndex++;
                }

                if (this->teethClearPixelFirstPassIndex < (8 * 8)) {
                    this->teethClearPixelFirstPassIndex++;
                }
            }
        }
    }

    if (this->dissolveTimer > (BREG(5) + 50)) {
        for (i = 0; i < (BREG(6) + 5); i++) {
            EnRaf_ClearPixelPetal(Lib_SegmentedToVirtual(&gCarnivorousLilyPadTrapPetalTex),
                                  sPetalClearPixelTableSecondPass, this->petalClearPixelSecondPassIndex);
            EnRaf_ClearPixelTeeth(Lib_SegmentedToVirtual(&gCarnivorousLilyPadTrapTeethTex),
                                  sTeethClearPixelTableSecondPass, this->teethClearPixelSecondPassIndex);
            if (this->petalClearPixelSecondPassIndex < (16 * 32)) {
                this->petalClearPixelSecondPassIndex++;
            }

            if (this->teethClearPixelSecondPassIndex < (8 * 8)) {
                this->teethClearPixelSecondPassIndex++;
            }
        }
    }

    if (this->dissolveTimer > (BREG(7) + 160)) {
        for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT; i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT;
             i++) {
            Math_Vec3f_Copy(&this->limbScale[i], &gZeroVec3f);
            Math_Vec3f_Copy(&this->targetLimbScale[i], &gZeroVec3f);
        }

        EnRaf_SetupDormant(this);
    }
}

void EnRaf_SetupDormant(EnRaf* this) {
    if (this->action == EN_RAF_ACTION_EXPLODE) {
        this->timer = 90;
    } else {
        this->action = EN_RAF_ACTION_DORMANT;
    }

    this->actionFunc = EnRaf_Dormant;
}

/**
 * Simply sits around doing nothing. If the revive timer is non-zero, then this function
 * will decrement the revive timer and revive the trap petals once it reaches 0.
 */
void EnRaf_Dormant(EnRaf* this, GlobalContext* globalCtx) {
    Vec3f targetLimbScale = { 1.0f, 1.0f, 1.0f };
    s32 i;

    if (this->timer == 0) {
        this->action = EN_RAF_ACTION_DORMANT;
    }

    if (this->reviveTimer >= 0) {
        DECR(this->reviveTimer);

        if (this->reviveTimer == 0) {
            EnRaf_ChangeAnimation(this, EN_RAF_ANIMATION_SPIT);

            for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT;
                 i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT; i++) {
                Math_Vec3f_Copy(&this->targetLimbScale[i], &targetLimbScale);
            }

            this->petalScaleType = EN_RAF_PETAL_SCALE_TYPE_IDLE_OR_THROW;
            this->action = EN_RAF_ACTION_IDLE;
            this->reviveTimer = EN_RAF_GET_REVIVE_TIMER(&this->dyna.actor);
            this->reviveTimer += 30;
            this->actionFunc = EnRaf_Idle;
        }
    }
}

void EnRaf_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnRaf* this = THIS;
    WaterBox* waterBox;
    f32 ySurface;
    Vec3f ripplePos;
    s32 i;

    SkelAnime_Update(&this->skelAnime);
    DECR(this->rippleTimer);
    DECR(this->timer);
    this->actionFunc(this, globalCtx);

    if ((this->action == EN_RAF_ACTION_IDLE) && (gSaveContext.save.weekEventReg[12] & 1)) {
        this->petalScaleType = EN_RAF_PETAL_SCALE_TYPE_DEAD;
        EnRaf_SetupConvulse(this);
        return;
    }

    if (DynaPolyActor_IsInRidingMovingState(&this->dyna)) {
        if ((this->heightDiffFromPlayer > -0.1f) && !this->isCurrentlyInRidingMovingState) {
            this->heightDiffFromPlayer = -20.0f;
            this->isCurrentlyInRidingMovingState = true;
            Actor_PlaySfxAtPos(&this->dyna.actor, NA_SE_EN_COMMON_WATER_MID);
        }
    } else {
        this->isCurrentlyInRidingMovingState = false;
    }

    this->bobPhase += 3000.0f;
    this->bobOffset = 2.0f * Math_SinS(this->bobPhase);
    if (this->mainType != EN_RAF_TYPE_NO_WATER_INTERACTIONS) {
        ySurface = BREG(60) + (this->dyna.actor.world.pos.y - 60.0f);
        if (WaterBox_GetSurface1(globalCtx, &globalCtx->colCtx, this->dyna.actor.world.pos.x,
                                 this->dyna.actor.world.pos.z, &ySurface, &waterBox)) {
            ySurface -= this->bobOffset + BREG(59);
            Math_ApproachF(&this->dyna.actor.world.pos.y, this->heightDiffFromPlayer + ySurface, 0.5f, 40.0f);
            if (this->rippleTimer == 0) {
                this->rippleTimer = 30;
                if (this->petalScaleType == EN_RAF_PETAL_SCALE_TYPE_CHEW) {
                    this->rippleTimer = 10;
                }

                Math_Vec3f_Copy(&ripplePos, &this->dyna.actor.world.pos);
                ripplePos.y = ySurface;
                EffectSsGRipple_Spawn(globalCtx, &ripplePos, 650, 3150, 0);
            }
        }
    } else {
        Math_ApproachF(&this->dyna.actor.world.pos.y,
                       (this->dyna.actor.home.pos.y + this->heightDiffFromPlayer) - this->bobOffset, 0.5f, 40.0f);
    }

    Math_ApproachZeroF(&this->heightDiffFromPlayer, 0.3f, 2.0f);
    if (this->action == EN_RAF_ACTION_EXPLODE) {
        EnRaf_UpdateParticles(this, globalCtx);
    }

    for (i = 0; i < ARRAY_COUNT(this->limbScale); i++) {
        if (this->action < EN_RAF_ACTION_EXPLODE) {
            Math_ApproachF(&this->limbScale[i].x, this->targetLimbScale[i].x, 0.4f, 0.5f);
            Math_ApproachF(&this->limbScale[i].y, this->targetLimbScale[i].y, 0.4f, 0.5f);
            Math_ApproachF(&this->limbScale[i].z, this->targetLimbScale[i].z, 0.4f, 0.5f);
        } else {
            Math_ApproachF(&this->limbScale[i].x, this->targetLimbScale[i].x, 1.0f, 1.0f);
            Math_ApproachF(&this->limbScale[i].y, this->targetLimbScale[i].y, 1.0f, 1.0f);
            Math_ApproachF(&this->limbScale[i].z, this->targetLimbScale[i].z, 1.0f, 1.0f);
        }
    }

    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    if (this->action < EN_RAF_ACTION_EXPLODE) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }
}

static s16 sGrabAnimationCheckFrames[] = { 0, 4, 6 };

static Vec3f sMiddleSegmentTargetScaleDuringGrab[] = {
    { 1.0f, 1.0f, 1.0f },
    { 1.0f, 2.0f, 1.0f },
    { 0.0f, 1.5f, 0.7f },
};

static Vec3f sUpperSegmentTargetScaleDuringGrab[] = {
    { 1.0f, 1.0f, 1.0f },
    { 3.0f, 2.0f, 1.5f },
    { 1.5f, 1.2f, 0.8f },
};

static s16 sSpitAnimationCheckFrames[] = { 0, 7, 9, 13, 19 };

static Vec3f sMiddleSegmentTargetScaleDuringSpit[] = {
    { 1.0f, 1.5f, 0.7f }, { 1.0f, 2.0f, 1.5f }, { 1.0f, 2.0f, 0.5f }, { 1.0f, 2.0f, 0.5f }, { 1.0f, 1.0f, 1.0f },
};

static Vec3f sUpperSegmentTargetScaleDuringSpit[] = {
    { 1.5f, 1.5f, 1.7f }, { 1.5f, 1.5f, 1.3f }, { 3.0f, 1.0f, 0.5f }, { 1.0f, 1.0f, 0.5f }, { 1.0f, 1.0f, 1.0f },
};

void EnRaf_TransformLimbDraw(GlobalContext* globalCtx2, s32 limbIndex, Actor* thisx) {
    GlobalContext* globalCtx = globalCtx2;
    EnRaf* this = THIS;
    s32 i;

    switch (this->petalScaleType) {
        case EN_RAF_PETAL_SCALE_TYPE_GRAB:
            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_MIDDLE_SEGMENT)) {
                for (i = 0; i < ARRAY_COUNT(sGrabAnimationCheckFrames); i++) {
                    if ((s16)this->skelAnime.curFrame == sGrabAnimationCheckFrames[i]) {
                        Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sMiddleSegmentTargetScaleDuringGrab[i]);
                    }
                }
            }

            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_UPPER_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_UPPER_SEGMENT)) {
                for (i = 0; i < ARRAY_COUNT(sGrabAnimationCheckFrames); i++) {
                    if ((s16)this->skelAnime.curFrame == sGrabAnimationCheckFrames[i]) {
                        Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sUpperSegmentTargetScaleDuringGrab[i]);
                    }
                }
            }
            break;

        case EN_RAF_PETAL_SCALE_TYPE_CHEW:
            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_MIDDLE_SEGMENT)) {
                Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sMiddleSegmentTargetScaleDuringGrab[2]);
            } else if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_UPPER_SEGMENT) ||
                       (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT) ||
                       (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_UPPER_SEGMENT)) {
                Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sUpperSegmentTargetScaleDuringGrab[2]);
            }

            // These matrix operations make the trap petals look a bit more "wobbly" as it chews
            // by stretching the limbs in various random directions.
            if ((limbIndex > CARNIVOROUS_LILY_PAD_LIMB_FLOWER) && (limbIndex < CARNIVOROUS_LILY_PAD_LIMB_ROOTS)) {
                Matrix_RotateY((this->chewLimbRot[limbIndex].y * globalCtx->gameplayFrames), MTXMODE_APPLY);
                Matrix_InsertXRotation_s((this->chewLimbRot[limbIndex].x * globalCtx->gameplayFrames), MTXMODE_APPLY);
                Matrix_InsertZRotation_s((this->chewLimbRot[limbIndex].z * globalCtx->gameplayFrames), MTXMODE_APPLY);
                Matrix_Scale(this->chewScale + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
                Matrix_InsertZRotation_s(-(this->chewLimbRot[limbIndex].z * globalCtx->gameplayFrames), MTXMODE_APPLY);
                Matrix_InsertXRotation_s(-(this->chewLimbRot[limbIndex].x * globalCtx->gameplayFrames), MTXMODE_APPLY);
                Matrix_RotateY(-(this->chewLimbRot[limbIndex].y * globalCtx->gameplayFrames), MTXMODE_APPLY);
            }
            break;

        case EN_RAF_PETAL_SCALE_TYPE_IDLE_OR_THROW:
            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_MIDDLE_SEGMENT)) {
                for (i = 0; i < ARRAY_COUNT(sSpitAnimationCheckFrames); i++) {
                    if ((s16)this->skelAnime.curFrame == sSpitAnimationCheckFrames[i]) {
                        Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sMiddleSegmentTargetScaleDuringSpit[i]);
                    }
                }
            }

            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_UPPER_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_UPPER_SEGMENT)) {
                for (i = 0; i < 4; i++) {
                    if ((s16)this->skelAnime.curFrame == sSpitAnimationCheckFrames[i]) {
                        Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sUpperSegmentTargetScaleDuringSpit[i]);
                    }
                }
            }
            break;
    }

    Matrix_Scale(this->limbScale[limbIndex].x, this->limbScale[limbIndex].y, this->limbScale[limbIndex].z,
                 MTXMODE_APPLY);
}

void EnRaf_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnRaf* this = THIS;

    func_8012C28C(globalCtx->state.gfxCtx);
    func_8012C2DC(globalCtx->state.gfxCtx);
    SkelAnime_DrawTransformFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, NULL, EnRaf_TransformLimbDraw, &this->dyna.actor);

    if (this->action == EN_RAF_ACTION_EXPLODE) {
        EnRaf_DrawParticles(this, globalCtx);
    }
}

void EnRaf_InitializeParticle(EnRaf* this, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale,
                              s16 timer) {
    s16 i;
    EnRafParticle* particle = this->particles;

    for (i = 0; i < ARRAY_COUNT(this->particles); i++, particle++) {
        if (!particle->isVisible) {
            particle->isVisible = true;
            particle->position = *position;
            particle->velocity = *velocity;
            particle->acceleration = *acceleration;
            particle->scale = scale;
            particle->timer = timer;
            particle->rotation.x = randPlusMinusPoint5Scaled(30000.0f);
            particle->rotation.y = randPlusMinusPoint5Scaled(30000.0f);
            particle->rotation.z = randPlusMinusPoint5Scaled(30000.0f);
            return;
        }
    }
}

void EnRaf_UpdateParticles(EnRaf* this, GlobalContext* globalCtx) {
    s32 i;
    EnRafParticle* particle = this->particles;

    for (i = 0; i < ARRAY_COUNT(this->particles); i++, particle++) {
        if (particle->isVisible) {
            particle->position.x += particle->velocity.x;
            particle->position.y += particle->velocity.y;
            particle->position.z += particle->velocity.z;
            particle->rotation.x += 0xBB8;
            particle->rotation.y += 0xBB8;
            particle->rotation.z += 0xBB8;
            particle->velocity.x += particle->acceleration.x;
            particle->velocity.y += particle->acceleration.y;
            particle->velocity.z += particle->acceleration.z;

            if (this->mainType != EN_RAF_TYPE_NO_WATER_INTERACTIONS) {
                if (particle->position.y < (this->dyna.actor.world.pos.y - 10.0f)) {
                    EffectSsGSplash_Spawn(globalCtx, &particle->position, NULL, NULL, 0, particle->scale * 200000.0f);
                    SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &particle->position, 50, NA_SE_EV_BOMB_DROP_WATER);
                    particle->isVisible = false;
                }
            } else if (particle->position.y < (this->dyna.actor.world.pos.y - 10.0f)) {
                Math_ApproachZeroF(&particle->scale, 0.2f, 0.001f);
                if (particle->scale <= 0.0001f) {
                    particle->timer = 0;
                }
            }

            if (particle->timer != 0) {
                particle->timer--;
            } else {
                particle->isVisible = false;
            }
        }
    }
}

void EnRaf_DrawParticles(EnRaf* this, GlobalContext* globalCtx) {
    s16 i;
    EnRafParticle* particle = this->particles;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    func_8012C28C(globalCtx->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->particles); i++, particle++) {
        if (particle->isVisible) {
            Matrix_InsertTranslation(particle->position.x, particle->position.y, particle->position.z, MTXMODE_NEW);
            Matrix_Scale(particle->scale, particle->scale, particle->scale, MTXMODE_APPLY);
            Matrix_InsertXRotation_s(particle->rotation.x, MTXMODE_APPLY);
            Matrix_RotateY(particle->rotation.y, MTXMODE_APPLY);
            Matrix_InsertZRotation_s(particle->rotation.z, MTXMODE_APPLY);

            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gCarnivorousLilyPadParticleDL);
        }
    }

    CLOSE_DISPS(gfxCtx);
}
