/*
 * File: z_bg_open_spot.c
 * Overlay: ovl_Bg_Open_Spot
 * Description: Spotlights that shine on the Skull Kid when he turns Link
 * into a Deku Scrub.
 */
#include "z_bg_open_spot.h"
#include "objects/object_open_obj/object_open_obj.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((BgOpenSpot*)thisx)

void BgOpenSpot_Init(Actor* thisx, GlobalContext* globalCtx);
void BgOpenSpot_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgOpenSpot_Update(Actor* thisx, GlobalContext* globalCtx);
void BgOpenSpot_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Bg_Open_Spot_InitVars = {
    ACTOR_BG_OPEN_SPOT,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_OPEN_OBJ,
    sizeof(BgOpenSpot),
    (ActorFunc)BgOpenSpot_Init,
    (ActorFunc)BgOpenSpot_Destroy,
    (ActorFunc)BgOpenSpot_Update,
    (ActorFunc)NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 560, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgOpenSpot_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgOpenSpot* this = THIS;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->texScrolls = Lib_SegmentedToVirtual(gSpotlightTexAnim);
}

void BgOpenSpot_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void BgOpenSpot_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgOpenSpot* this = THIS;
    u32 action;

    if (Cutscene_CheckActorAction(globalCtx, 0x7D)) {
        action = globalCtx->csCtx.actorActions[Cutscene_GetActorActionIndex(globalCtx, 0x7D)]->action;
        if (action == 1) {
            this->actor.draw = NULL;
        } else if (action == 2) {
            this->actor.draw = BgOpenSpot_Draw;
        }
    }
}

void BgOpenSpot_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgOpenSpot* this = (BgOpenSpot*)thisx;

    AnimatedMat_Draw(globalCtx, this->texScrolls);
    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C2DC(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_BAYER);
    gSPDisplayList(POLY_XLU_DISP++, gSpotlightLeftDL);
    gSPDisplayList(POLY_XLU_DISP++, gSpotlightRightDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
