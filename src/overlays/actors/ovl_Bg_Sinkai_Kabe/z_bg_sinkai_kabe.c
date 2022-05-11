/*
 * File: z_bg_sinkai_kabe.c
 * Overlay: ovl_Bg_Sinkai_Kabe
 * Description: Manages the Deep Pythons in Pinnacle Rock
 *
 * While its name describes one of its responsibilities ("Deep Sea Wall", aka the invisible
 * wall blocking a Deep Python's den before you defeat it), it is also responsible for
 * spawning in the Deep Pythons when the area first loads, as well as signalling to the
 * Deep Python that it should extend its body. Furthermore, if the player defeats all
 * eight Deep Pythons and spawns the second Seahorse, this actor is responsible for spawning
 * it again if the player leaves and re-enters Pinnacle Rock.
 */

#include "z_bg_sinkai_kabe.h"
#include "objects/object_sinkai_kabe/object_sinkai_kabe.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((BgSinkaiKabe*)thisx)

void BgSinkaiKabe_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSinkaiKabe_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSinkaiKabe_Update(Actor* thisx, GlobalContext* globalCtx);

void BgSinkaiKabe_WaitForPlayer(BgSinkaiKabe* this, GlobalContext* globalCtx);

const ActorInit Bg_Sinkai_Kabe_InitVars = {
    ACTOR_BG_SINKAI_KABE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_SINKAI_KABE,
    sizeof(BgSinkaiKabe),
    (ActorFunc)BgSinkaiKabe_Init,
    (ActorFunc)BgSinkaiKabe_Destroy,
    (ActorFunc)BgSinkaiKabe_Update,
    (ActorFunc)NULL,
};

static s32 sCurrentPythonIndex = 0;

void BgSinkaiKabe_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSinkaiKabe* this = THIS;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    Vec3f pos;
    s32 pad2;
    s32 shouldSpawnSeahorse;
    s32 cs;
    s32 i;

    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gPinnacleRockPythonDenCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    Math_Vec3f_Copy(&pos, &this->dyna.actor.world.pos);
    pos.x += Math_SinS(this->dyna.actor.world.rot.y + 0x8000) * 3000.0f;
    pos.z += Math_CosS(this->dyna.actor.world.rot.y + 0x8000) * 3000.0f;
    cs = this->dyna.actor.cutscene;
    i = 0;

    // clang-format off
    while (cs != -1) { this->cutscenes[i] = cs; cs = ActorCutscene_GetAdditionalCutscene(cs); i++; }
    // clang-format on

    this->dyna.actor.scale.x = 0.1f;
    this->dyna.actor.scale.y = 0.1f;
    this->dyna.actor.scale.z = 0.1f;
    this->pythonIndex = sCurrentPythonIndex;
    sCurrentPythonIndex++;

    if (!(gSaveContext.save.weekEventReg[13] & 1)) {
        this->deepPython =
            Actor_SpawnAsChild(&globalCtx->actorCtx, &this->dyna.actor, globalCtx, ACTOR_EN_DRAGON, pos.x, pos.y, pos.z,
                               0, this->dyna.actor.world.rot.y, 1, this->dyna.actor.params);

        if (this->deepPython != NULL) {
            EnDragon* dragon = (EnDragon*)this->deepPython;

            dragon->grabCutsceneIndex = this->cutscenes[0];
            dragon->deathCutsceneIndex = this->cutscenes[1];
            dragon->actor.cutscene = this->dyna.actor.cutscene;
            Math_Vec3f_Copy(&dragon->burrowEntrancePos, &this->dyna.actor.world.pos);
            dragon->pythonIndex = this->pythonIndex;
        }
    } else {
        shouldSpawnSeahorse = false;
        if (((this->dyna.actor.params == 0) && (gSaveContext.save.weekEventReg[83] & 0x10)) ||
            ((this->dyna.actor.params == 1) && (gSaveContext.save.weekEventReg[83] & 0x20)) ||
            ((this->dyna.actor.params == 2) && (gSaveContext.save.weekEventReg[83] & 0x40)) ||
            ((this->dyna.actor.params == 3) && (gSaveContext.save.weekEventReg[83] & 0x80)) ||
            ((this->dyna.actor.params == 4) && (gSaveContext.save.weekEventReg[84] & 1)) ||
            ((this->dyna.actor.params == 5) && (gSaveContext.save.weekEventReg[84] & 2)) ||
            ((this->dyna.actor.params == 6) && (gSaveContext.save.weekEventReg[84] & 4)) ||
            ((this->dyna.actor.params == 7) && (gSaveContext.save.weekEventReg[84] & 8))) {
            shouldSpawnSeahorse = true;
        }

        Math_Vec3f_Copy(&pos, &this->dyna.actor.home.pos);
        pos.x += (Math_SinS(this->dyna.actor.world.rot.y + 0x8000) * 500.0f);
        pos.y += -100.0f;
        pos.z += (Math_CosS(this->dyna.actor.world.rot.y + 0x8000) * 500.0f);
        if (shouldSpawnSeahorse) {
            Actor_SpawnAsChildAndCutscene(&globalCtx->actorCtx, globalCtx, ACTOR_EN_OT, pos.x, pos.y, pos.z, 0,
                                          this->dyna.actor.shape.rot.y, 0, 0x4000, this->dyna.actor.cutscene,
                                          this->dyna.actor.unk20, NULL);
        }

        Actor_MarkForDeath(&this->dyna.actor);
    }

    this->actionFunc = BgSinkaiKabe_WaitForPlayer;
}

void BgSinkaiKabe_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSinkaiKabe* this = THIS;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

/**
 * Extends this actor's Deep Python if the player gets close enough.
 */
void BgSinkaiKabe_WaitForPlayer(BgSinkaiKabe* this, GlobalContext* globalCtx) {
    if (this->deepPython != NULL) {
        if (this->deepPython->update != NULL) {
            EnDragon* dragon = (EnDragon*)this->deepPython;

            if (dragon->action == DEEP_PYTHON_ACTION_IDLE) {
                Player* player = GET_PLAYER(globalCtx);

                if ((this->dyna.actor.xzDistToPlayer < 500.0f) ||
                    ((ABS_ALT(BINANG_SUB(this->dyna.actor.yawTowardsPlayer, this->dyna.actor.world.rot.y)) < 0x2000) &&
                     (this->dyna.actor.xzDistToPlayer < 700.0f))) {
                    if (fabsf(this->dyna.actor.world.pos.y - player->actor.world.pos.y) < 400.0f) {
                        dragon->action = DEEP_PYTHON_ACTION_EXTEND;
                    }
                }
            }
        } else {
            this->deepPython = NULL;
        }
    }

    if (this->deepPython == NULL) {
        Actor_MarkForDeath(&this->dyna.actor);
    }
}

void BgSinkaiKabe_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSinkaiKabe* this = THIS;

    this->actionFunc(this, globalCtx);
}
