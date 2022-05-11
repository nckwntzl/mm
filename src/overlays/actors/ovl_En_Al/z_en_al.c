/*
 * File: z_en_al.c
 * Overlay: ovl_En_Al
 * Description: Madame Aroma
 */

#include "z_en_al.h"
#include "objects/object_al/object_al.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_10 | ACTOR_FLAG_20)

#define THIS ((EnAl*)thisx)

void EnAl_Init(Actor* thisx, GlobalContext* globalCtx);
void EnAl_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnAl_Update(Actor* thisx, GlobalContext* globalCtx);
void EnAl_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80BDF5E8(EnAl* this, GlobalContext* globalCtx);
void func_80BDF6C4(EnAl* this, GlobalContext* globalCtx);

s32 D_80BDFC70[] = {
    0x0C00030D, 0x02120006, 0x0001050E, 0x12000600, 0x02020800, 0x0A002F02, 0x0A000A37,
    0x23020A37, 0x0B2D0D02, 0x0B2D1400, 0x01050E0B, 0x2D140001, 0x0C000206, 0x0E0A370B,
    0x2D010E0A, 0x370B2D03, 0x0E0A000A, 0x37010400, 0x01000000,
};

s32 D_80BDFCBC[] = {
    0x09000017, 0x0E27A50C, 0x09000018, 0x0E27A60C, 0x09000017, 0x0E27A70C, 0x09000018, 0x0E27A80C,
    0x09000017, 0x0E27A90C, 0x09000018, 0x0E27AA0C, 0x09000017, 0x0E27AB0C, 0x09000018, 0x0E27AC0C,
    0x09000017, 0x0E27AD2D, 0x000B2D00, 0x080C1159, 0x08090000, 0x10000000,
};

s32 D_80BDFD14[] = {
    0x00562000, 0xA2090000, 0x170E2AA6, 0x0C090000, 0x180E2AA7, 0x0C090000, 0x170E2AA8, 0x0C090000,
    0x180E2AA9, 0x0C090000, 0x170E2AAA, 0x0C090000, 0x180E2AAB, 0x0C090000, 0x170E2AAC, 0x0C090000,
    0x170E2AAD, 0x0C0F2AAE, 0x0C090000, 0x180E2AAF, 0x0C090000, 0x170E2AB0, 0x0C090000, 0x170E2AB1,
    0x0C090000, 0x180E2AB2, 0x0C090000, 0x170E2AB3, 0x0C090000, 0x180E2AB4, 0x0C090000, 0x170E2AB5,
    0x0C090000, 0x180E2AB6, 0x0C090000, 0x170E2AB7, 0x0C090000, 0x180E2AB8, 0x2D00082D, 0x00092D00,
    0x0A0C1156, 0x20121009, 0x0000170E, 0x2AEA0C09, 0x0000180E, 0x2AEB0C09, 0x00001210,
};

s32 D_80BDFDD0[] = { 0x005C0800, 0x0B0E2AE5, 0x2D000811, 0x5C080C10, 0x0E2AE62D, 0x00080C10 };

s32 D_80BDFDE8[] = { 0x0900000E, 0x2B192D00, 0x080C1509, 0x00001210 };

s32 D_80BDFDF8[] = {
    0x09000000, 0x39040071, 0x0E2B1A0C, 0x0F00FF1E, 0x00330018, 0x000D0000, 0x0E2B1B2D, 0x00080C15, 0x09000012,
    0x102C2B1C, 0x0C2F0000, 0x0C19FFDC, 0x2C2B1D0C, 0x2F00000C, 0x0F2B1E0C, 0x0F2B1F0C, 0x122A002E, 0x00390800,
    0x1306006F, 0x00001300, 0x6F2F0000, 0x2E2D0023, 0x0C07000C, 0x06000500, 0x00130005, 0x0C070000, 0x0E2B202D,
    0x00080C16, 0x11390411, 0x390819FF, 0xA60E2B3C, 0x2D00080C, 0x19FF9C00,
};

s32 D_80BDFE7C[] = { 0x0E2A9C2D, 0x00080C10 };

s32 D_80BDFE84[] = { 0x0E2A9D2D, 0x00080C10 };

s32 D_80BDFE8C[] = {
    0x00390200, 0x0F0E2A9E, 0x0C0F2A9F, 0x0C0F2AA0, 0x0C190004, 0x0E2AA00C, 0x05000000, 0x1A001A30, 0x0E2AA20C,
    0x1206008F, 0x00001300, 0x8F2F0000, 0x2E2D002C, 0x2D00080C, 0x10310E2A, 0xA12D0008, 0x0C113902, 0x10000000,
};

s32 D_80BDFED4[] = {
    0x0900002C, 0x2B1D0C2F, 0x00000C0F, 0x2B1E0C0F, 0x2B1F0C12, 0x2A002E00, 0x39080013,
    0x06006F00, 0x0013006F, 0x2F00002E, 0x2D00230C, 0x07000C06, 0x00050000, 0x1300050C,
    0x0700000E, 0x2B202D00, 0x080C1611, 0x39041139, 0x08150900, 0x00121000,
};

s32 D_80BDFF24[] = {
    0x0900002C, 0x2B1C0C0F, 0x00FF1E00, 0x33000DFF, 0xF000000E, 0x2B1B2D00, 0x080C1509, 0x00001210,
    0x2C2B1D0C, 0x2F00000C, 0x0F2B1E0C, 0x0F2B1F0C, 0x122A002E, 0x00390800, 0x1306006F, 0x00001300,
    0x6F2F0000, 0x2E2D0023, 0x0C07000C, 0x06000500, 0x00130005, 0x0C070000, 0x0E2B202D, 0x00080C16,
    0x11390411, 0x390819FF, 0xB10E2B3C, 0x2D00080C, 0x19FFA700,
};

const ActorInit En_Al_InitVars = {
    ACTOR_EN_AL,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_AL,
    sizeof(EnAl),
    (ActorFunc)EnAl_Init,
    (ActorFunc)EnAl_Destroy,
    (ActorFunc)EnAl_Update,
    (ActorFunc)EnAl_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT1,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 14, 62, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static AnimationInfoS sAnimations[] = {
    { &object_al_Anim_000C54, 1.0f, 0, -1, 0, -4 }, { &object_al_Anim_00DBE0, 1.0f, 0, -1, 0, 0 },
    { &object_al_Anim_00DBE0, 1.0f, 0, -1, 0, -4 }, { &object_al_Anim_00ACA0, 1.0f, 0, -1, 2, 0 },
    { &object_al_Anim_00ACA0, -1.0f, 0, -1, 2, 0 }, { &object_al_Anim_00CA28, 1.0f, 0, -1, 0, -4 },
    { &object_al_Anim_00BCA4, 1.0f, 0, -1, 2, 0 },  { &object_al_Anim_00A764, 1.0f, 0, -1, 0, -4 },
};

Vec3f D_80BE0070 = { 1000.0f, 0.0f, 0.0f };

Gfx* D_80BE007C[] = {
    object_al_DL_006598, object_al_DL_005920, object_al_DL_005878,
    object_al_DL_0057D0, object_al_DL_005728, object_al_DL_005680,
};

Actor* func_80BDE1A0(EnAl* this, GlobalContext* globalCtx, u8 arg0, s16 arg1) {
    Actor* foundActor = NULL;
    Actor* temp;

    while (true) {
        foundActor = SubS_FindActor(globalCtx, foundActor, arg0, arg1);

        if (foundActor == NULL) {
            break;
        }

        if ((this != (EnAl*)foundActor) && (foundActor->update != NULL)) {
            break;
        }

        temp = foundActor->next;
        if (temp == NULL) {
            foundActor = NULL;
            break;
        }
        foundActor = temp;
    }
    return foundActor;
}

void func_80BDE250(EnAl* this) {
    this->skelAnime.playSpeed = this->unk_4C8;
    SkelAnime_Update(&this->skelAnime);
}

s32 func_80BDE27C(EnAl* this, s32 arg1) {
    s32 phi_v1 = false;
    s32 ret = false;

    if ((arg1 == 1) || (arg1 == 2)) {
        if ((this->unk_4F8 != 1) && (this->unk_4F8 != 2)) {
            phi_v1 = true;
        }
    } else if (arg1 != this->unk_4F8) {
        phi_v1 = true;
    }

    if (phi_v1) {
        this->unk_4F8 = arg1;
        ret = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimations, arg1);
        this->unk_4C8 = this->skelAnime.playSpeed;
    }

    return ret;
}

void func_80BDE318(EnAl* this, GlobalContext* globalCtx) {
    s32 pad;
    f32 temp;

    Collider_UpdateCylinder(&this->actor, &this->unk_310);
    temp = this->actor.focus.pos.y - this->actor.world.pos.y;
    this->unk_310.dim.height = temp;
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->unk_310.base);
}

Actor* func_80BDE384(EnAl* this, GlobalContext* globalCtx) {
    Actor* actor;

    switch (this->unk_35C) {
        case 2:
            if (!(gSaveContext.save.weekEventReg[89] & 8) && (gSaveContext.save.weekEventReg[85] & 0x80)) {
                actor = func_80BDE1A0(this, globalCtx, ACTORCAT_NPC, ACTOR_EN_PM);
            } else {
                actor = &GET_PLAYER(globalCtx)->actor;
            }
            break;

        case 3:
            actor = this->unk_368;
            break;

        default:
            actor = &GET_PLAYER(globalCtx)->actor;
            break;
    }

    return actor;
}

s32 func_80BDE408(EnAl* this, s16 arg1) {
    s32 ret = false;

    if (ActorCutscene_GetCurrentIndex() == 0x7C) {
        ActorCutscene_Stop(0x7C);
        ActorCutscene_SetIntentToPlay(arg1);
    } else if (ActorCutscene_GetCanPlayNext(arg1)) {
        ActorCutscene_StartAndSetUnkLinkFields(arg1, &this->actor);
        ret = true;
    } else {
        ActorCutscene_SetIntentToPlay(arg1);
    }
    return ret;
}

s16 func_80BDE484(EnAl* this, s32 arg1) {
    s16 cs = this->actor.cutscene;
    s32 i;

    for (i = 0; i < arg1; i++) {
        cs = ActorCutscene_GetAdditionalCutscene(cs);
    }

    return cs;
}

s32 func_80BDE4E0(EnAl* this, s16* arg1, s16 arg2) {
    s32 ret = false;
    s16 sp42;
    Vec3f sp34;
    Vec3f sp28;
    s32 temp;

    if ((this->unk_368 == NULL) || (this->unk_368->update == NULL)) {
        return true;
    }

    if (arg2 == *arg1) {
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_CHAIR_ROLL);
        func_80BDE27C(this, 3);
        this->unk_4E8 = 0;
        (*arg1)++;
    }

    if ((arg2 + 1) == *arg1) {
        Math_Vec3f_Copy(&sp28, &this->unk_368->world.pos);
        Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
        sp42 = Math_Vec3f_Yaw(&sp34, &sp28);
        temp = sp42 / 364;
        this->unk_4E8++;
        if ((temp != (this->actor.world.rot.y / 364)) && (this->unk_4E8 < 20)) {
            Math_ApproachS(&this->actor.world.rot.y, sp42, 3, 0x2AA8);
        } else {
            func_80BDE27C(this, 5);
            this->actor.world.rot.y = sp42;
            this->unk_4E8 = 0;
            (*arg1)++;
            ret = true;
        }
    }
    return ret;
}

s32 func_80BDE678(EnAl* this, s16* arg1, s16 arg2) {
    s32 ret = false;
    s16 sp22;
    s32 temp;

    if ((this->unk_368 == NULL) || (this->unk_368->update == NULL)) {
        return true;
    }

    if (arg2 == *arg1) {
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_CHAIR_ROLL);
        func_80BDE27C(this, 4);
        this->unk_4E8 = 0;
        (*arg1)++;
    }

    if ((arg2 + 1) == *arg1) {
        sp22 = this->actor.home.rot.y;
        temp = sp22 / 364;
        this->unk_4E8++;
        if ((temp != (this->actor.world.rot.y / 364)) && (this->unk_4E8 < 20)) {
            Math_ApproachS(&this->actor.world.rot.y, sp22, 3, 0x2AA8);
        } else {
            func_80BDE27C(this, 2);
            this->actor.world.rot.y = sp22;
            this->unk_4E8 = 0;
            (*arg1)++;
            ret = true;
        }
    }
    return ret;
}

s32 func_80BDE7FC(EnAl* this, GlobalContext* globalCtx) {
    s32 pad;
    s16 sp2A = func_80BDE484(this, 0);
    s32 pad2;
    s32 sp20 = false;

    switch (this->unk_4E6) {
        case 0:
            if (!func_80BDE408(this, sp2A)) {
                break;
            }

        case 2:
        case 4:
        case 6:
        case 8:
            if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
                Camera_SetTargetActor(Play_GetCamera(globalCtx, ActorCutscene_GetCurrentCamera(sp2A)),
                                      this->actor.child);
            }
            this->unk_4E6++;
            sp20 = true;
            break;

        case 1:
        case 3:
        case 5:
        case 7:
            Camera_SetTargetActor(Play_GetCamera(globalCtx, ActorCutscene_GetCurrentCamera(sp2A)), &this->actor);
            this->unk_4E6++;
            sp20 = true;
            break;

        case 9:
            ActorCutscene_Stop(sp2A);
            this->unk_4E6++;
            sp20 = true;
            break;
    }
    return sp20;
}

s32 func_80BDE92C(EnAl* this, GlobalContext* globalCtx) {
    s32 pad[2];
    Actor* sp1C = func_80BDE1A0(this, globalCtx, ACTORCAT_NPC, ACTOR_EN_GM);
    Actor* temp_v0 = func_80BDE1A0(this, globalCtx, ACTORCAT_NPC, ACTOR_EN_TOTO);

    if ((sp1C == NULL) || (sp1C->update == NULL) || (temp_v0 == NULL) || (temp_v0->update == NULL)) {
        this->unk_4E6++;
        return true;
    }

    switch (this->unk_4E6) {
        case 0:
        case 1:
        case 3:
        case 5:
        case 8:
        case 10:
        case 11:
        case 13:
        case 15:
        case 17:
            this->actor.child = sp1C;
            this->unk_4E6++;
            break;

        case 7:
        case 9:
        case 12:
            this->actor.child = temp_v0;
            this->unk_4E6++;
            break;

        case 2:
        case 4:
        case 6:
        case 14:
        case 16:
            this->unk_4E6++;
            break;

        default:
            this->unk_4E6++;
            break;
    }
    return true;
}

s32 func_80BDEA14(EnAl* this, GlobalContext* globalCtx) {
    s32 pad;
    s32 sp18 = false;

    switch (this->unk_4E6) {
        case 0:
        case 1:
            if ((gSaveContext.save.weekEventReg[75] & 2)) {
                sp18 = true;
            } else if (func_80BDE4E0(this, &this->unk_4E6, 0)) {
                sp18 = true;
            }
            break;

        case 2:
        case 3:
            if (func_80BDE678(this, &this->unk_4E6, 2)) {
                sp18 = true;
            }
            break;
    }
    return sp18;
}

s32* func_80BDEABC(EnAl* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (this->unk_35C == 3) {
        this->unk_4EC = func_80BDE92C;
        return D_80BDFD14;
    }

    switch (this->unk_35C) {
        case 1:
            if (player->transformation == PLAYER_FORM_DEKU) {
                return D_80BDFDD0;
            }

            if (INV_CONTENT(ITEM_MASK_KAFEIS_MASK) != ITEM_MASK_KAFEIS_MASK) {
                return D_80BDFE8C;
            }

            if (Player_GetMask(globalCtx) == PLAYER_MASK_KAFEIS_MASK) {
                return D_80BDFE7C;
            }
            return D_80BDFE84;

        case 2:
            if (!(gSaveContext.save.weekEventReg[89] & 8) && (gSaveContext.save.weekEventReg[85] & 0x80)) {
                this->unk_4EC = func_80BDE7FC;
                return D_80BDFCBC;
            }

            this->unk_4EC = func_80BDEA14;
            if (Player_GetMask(globalCtx) != PLAYER_MASK_KAFEIS_MASK) {
                return D_80BDFDE8;
            }

            if (this->unk_4C2 & 0x800) {
                return D_80BDFED4;
            }

            if (this->unk_4C2 & 0x1000) {
                return D_80BDFF24;
            }
            return D_80BDFDF8;
    }
    return NULL;
}

s32 func_80BDEC2C(EnAl* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 ret = false;

    if ((this->unk_4C2 & 7) && Actor_ProcessTalkRequest(&this->actor, &globalCtx->state)) {
        this->unk_4C2 &= ~0x1800;
        if (player->exchangeItemId == 0x33) {
            this->unk_4C2 |= 0x800;
            this->unk_4F4 = player->exchangeItemId;
        } else if (player->exchangeItemId != 0) {
            this->unk_4C2 |= 0x1000;
            this->unk_4F4 = player->exchangeItemId;
        }
        SubS_UpdateFlags(&this->unk_4C2, 0, 7);
        this->unk_4E6 = 0;
        this->unk_4EC = 0;
        this->actor.child = this->unk_368;
        this->unk_360 = func_80BDEABC(this, globalCtx);
        this->unk_4C2 |= 0x20;
        this->actionFunc = func_80BDF6C4;
        ret = true;
    }

    return ret;
}

void func_80BDED20(EnAl* this) {
    s32 pad;
    Vec3f sp40;
    Vec3f sp34;
    s32 pad2;

    Math_Vec3f_Copy(&sp40, &this->unk_368->world.pos);
    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);

    Math_ApproachS(&this->unk_4DE, Math_Vec3f_Yaw(&sp34, &sp40) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_4DE = CLAMP(this->unk_4DE, -0x1FFE, 0x1FFE);

    Math_Vec3f_Copy(&sp34, &this->actor.focus.pos);

    if (this->unk_368->id == ACTOR_PLAYER) {
        sp40.y = ((Player*)this->unk_368)->bodyPartsPos[7].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&sp40, &this->unk_368->focus.pos);
    }

    Math_ApproachS(&this->unk_4DC, Math_Vec3f_Pitch(&sp34, &sp40), 4, 0x2AA8);
    this->unk_4DC = CLAMP(this->unk_4DC, -0x1554, 0x1554);
}

void func_80BDEE5C(EnAl* this) {
    if (this->unk_4C2 & 0x20) {
        if ((this->unk_368 != NULL) && (this->unk_368->update != NULL)) {
            if (DECR(this->unk_4E2) == 0) {
                func_80BDED20(this);
                this->unk_4C2 &= ~0x200;
                this->unk_4C2 |= 0x80;
                return;
            }
        }
    }

    if (this->unk_4C2 & 0x80) {
        this->unk_4C2 &= ~0x80;
        this->unk_4DC = 0;
        this->unk_4DE = 0;
        this->unk_4E2 = 0x14;
    } else if (DECR(this->unk_4E2) == 0) {
        this->unk_4C2 |= 0x200;
    }
}

void func_80BDEF3C(EnAl* this, GlobalContext* globalCtx) {
    if (this->unk_4E4 == 0) {
        func_80BDE27C(this, 7);
        this->unk_4C2 &= ~0x20;
        this->unk_4C2 |= 0x200;
        this->unk_4E4++;
    } else if ((this->unk_4E4 == 1) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        func_80BDE27C(this, 0);
        this->unk_4C2 &= ~0x200;
        this->unk_4C2 |= 0x20;
        this->unk_4E4++;
    }
}

void func_80BDEFE4(EnAl* this, GlobalContext* globalCtx) {
    if (this->unk_4E4 == 0) {
        func_80BDE27C(this, 6);
        this->unk_4E4 += 1;
    } else if ((this->unk_4E4 == 1) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        func_80BDE27C(this, 5);
        this->unk_4E4 += 1;
    }
}

s32 func_80BDF064(EnAl* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    u16 sp22 = globalCtx->msgCtx.currentTextId;
    Actor* sp1C = func_80BDE1A0(this, globalCtx, 4, 0xA4);
    Actor* temp_v0 = func_80BDE1A0(this, globalCtx, 4, 0x234);

    if (player->stateFlags1 & 0x40) {
        this->unk_4C2 |= 0x400;
        if (this->unk_4C4 != sp22) {
            switch (sp22) {
                case 0x2AA6:
                case 0x2AAF:
                case 0x2AB4:
                    if ((sp1C != NULL) && (sp1C->update != NULL)) {
                        this->unk_368 = sp1C;
                    }
                    break;

                case 0x2AAD:
                case 0x2AB0:
                    if ((temp_v0 != NULL) && (temp_v0->update != NULL)) {
                        this->unk_368 = temp_v0;
                    }
                    break;
            }

            switch (sp22) {
                case 0x2AA1:
                case 0x2AA2:
                case 0x2AA7:
                case 0x2AE6:
                case 0x2AE8:
                case 0x2AE9:
                case 0x2AB4:
                    this->unk_18C = func_80BDEF3C;
                    this->unk_4E4 = 0;
                    break;

                case 0x27A6:
                case 0x27A8:
                case 0x27AA:
                case 0x2B1E:
                    this->unk_18C = func_80BDEFE4;
                    this->unk_4E4 = 0;
                    break;

                case 0x2B19:
                case 0x2B20:
                case 0x2B3C:
                    func_80BDE27C(this, 5);
                    break;
            }
        }
        this->unk_4C4 = sp22;
    } else if (this->unk_4C2 & 0x400) {
        this->unk_4C4 = 0;
        this->unk_4C2 &= ~0x400;
    }

    if (this->unk_18C != NULL) {
        this->unk_18C(this, globalCtx);
    }

    return false;
}

s32 func_80BDF244(EnAl* this, GlobalContext* globalCtx, struct_80133038_arg2* arg2) {
    s32 ret = false;
    Actor* sp20 = func_80BDE1A0(this, globalCtx, ACTORCAT_NPC, ACTOR_EN_GM);
    Actor* temp_v0 = func_80BDE1A0(this, globalCtx, ACTORCAT_NPC, ACTOR_EN_TOTO);

    if ((sp20 != NULL) && (sp20->update != NULL) && (temp_v0 != NULL) && (temp_v0->update != NULL)) {
        func_80BDE27C(this, 0);
        SubS_UpdateFlags(&this->unk_4C2, 3, 7);
        this->unk_368 = sp20;
        this->unk_4C2 |= 0x20;
        ret = true;
    }
    return ret;
}

s32 func_80BDF308(EnAl* this, GlobalContext* globalCtx, struct_80133038_arg2* arg2) {
    SubS_UpdateFlags(&this->unk_4C2, 3, 7);

    switch (arg2->unk0) {
        case 1:
            func_80BDE27C(this, 0);
            break;

        case 2:
            this->unk_4F0 = EXCH_ITEM_NONE;
            this->unk_4EA = 0;
            func_80BDE27C(this, 2);
            break;
    }
    return true;
}

s32 func_80BDF390(EnAl* this, GlobalContext* globalCtx, struct_80133038_arg2* arg2) {
    s32 ret;

    this->actor.flags |= ACTOR_FLAG_1;
    this->actor.targetMode = 0;
    this->unk_4F0 = EXCH_ITEM_NONE;
    this->unk_4C2 = 0;
    this->unk_4D4 = 40.0f;

    switch (arg2->unk0) {
        case 3:
            ret = func_80BDF244(this, globalCtx, arg2);
            break;

        case 1:
        case 2:
            ret = func_80BDF308(this, globalCtx, arg2);
            break;

        default:
            ret = false;
            break;
    }
    return ret;
}

void func_80BDF414(EnAl* this, GlobalContext* globalCtx) {
    s16 temp_v0;

    switch (this->unk_4EA) {
        case 0:
        case 1:
            if (!(gSaveContext.save.weekEventReg[89] & 8)) {
                if (gSaveContext.save.weekEventReg[85] & 0x80) {
                    func_80BDE4E0(this, &this->unk_4EA, 0);
                }
            }
            break;

        case 2:
            if (gSaveContext.save.weekEventReg[89] & 8) {
                this->unk_4EA++;
            }
            break;

        case 3:
        case 4:
            func_80BDE678(this, &this->unk_4EA, 3);
            break;
    }

    temp_v0 = this->actor.world.rot.y - this->actor.yawTowardsPlayer;
    if (((this->unk_4EA == 0) && (ABS_ALT(temp_v0) >= 0x5800)) ||
        ((this->unk_4EA == 5) && (ABS_ALT(temp_v0) >= 0x5800)) || (this->unk_4EA == 2)) {
        SubS_UpdateFlags(&this->unk_4C2, 3, 7);
    } else {
        SubS_UpdateFlags(&this->unk_4C2, 0, 7);
    }
}

void func_80BDF568(EnAl* this, GlobalContext* globalCtx) {
}

void func_80BDF578(EnAl* this, GlobalContext* globalCtx) {
    switch (this->unk_35C) {
        case 1:
        case 3:
            func_80BDF568(this, globalCtx);
            break;

        case 2:
            func_80BDF414(this, globalCtx);
            break;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2AA8);
}

void func_80BDF5E8(EnAl* this, GlobalContext* globalCtx) {
    u32* unk14 = &gSaveContext.save.daySpeed;
    struct_80133038_arg2 sp20;

    this->unk_4E0 = REG(15) + *unk14;
    if (!func_80133038(globalCtx, D_80BDFC70, &sp20) ||
        ((this->unk_35C != sp20.unk0) && !func_80BDF390(this, globalCtx, &sp20))) {
        this->actor.shape.shadowDraw = NULL;
        this->actor.flags &= ~ACTOR_FLAG_1;
        sp20.unk0 = 0;
    } else {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.flags |= ACTOR_FLAG_1;
    }
    this->unk_35C = sp20.unk0;
    this->unk_368 = func_80BDE384(this, globalCtx);
    func_80BDF578(this, globalCtx);
}

void func_80BDF6C4(EnAl* this, GlobalContext* globalCtx) {
    if (func_8010BF58(&this->actor, globalCtx, this->unk_360, this->unk_4EC, &this->unk_364)) {
        SubS_UpdateFlags(&this->unk_4C2, 3, 7);
        this->unk_4C2 &= ~0x20;
        this->unk_4C2 |= 0x200;
        this->actor.child = NULL;
        this->unk_4E2 = 20;
        this->unk_364 = 0;
        this->actionFunc = func_80BDF5E8;
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2AA8);
    }
}

void EnAl_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnAl* this = THIS;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &object_al_Skel_00A0D8, NULL, this->jointTable, this->morphTable,
                       27);
    this->unk_4F8 = -1;
    func_80BDE27C(this, 1);
    Collider_InitAndSetCylinder(globalCtx, &this->unk_310, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.01f);
    this->unk_35C = 0;
    this->actionFunc = func_80BDF5E8;

    this->actionFunc(this, globalCtx);
}

void EnAl_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnAl* this = THIS;

    Collider_DestroyCylinder(globalCtx, &this->unk_310);
}

void EnAl_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnAl* this = THIS;

    func_80BDEC2C(this, globalCtx);

    this->actionFunc(this, globalCtx);

    func_80BDF064(this, globalCtx);

    if (this->unk_35C != 0) {
        func_80BDE250(this);
        func_80BDEE5C(this);
        func_8013C964(&this->actor, globalCtx, this->unk_4D4, 30.0f, this->unk_4F0, this->unk_4C2 & 7);
        func_80BDE318(this, globalCtx);
    }
}

s32 EnAl_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    switch (limbIndex) {
        case 3:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            *dList = NULL;
            break;

        case 16:
            break;
    }
    return false;
}

void EnAl_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnAl* this = THIS;

    switch (limbIndex) {
        case 3:
            Matrix_CopyCurrentState(&this->unk_190[0]);
            break;

        case 11:
            Matrix_CopyCurrentState(&this->unk_190[1]);
            break;

        case 12:
            Matrix_CopyCurrentState(&this->unk_190[2]);
            break;

        case 13:
            Matrix_CopyCurrentState(&this->unk_190[3]);
            break;

        case 14:
            Matrix_CopyCurrentState(&this->unk_190[4]);
            break;

        case 15:
            Matrix_CopyCurrentState(&this->unk_190[5]);
            break;

        case 16:
            Matrix_MultiplyVector3fByState(&D_80BE0070, &this->actor.focus.pos);
            Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
            break;
    }
}

void EnAl_TransformLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Actor* thisx) {
    EnAl* this = THIS;
    s32 phi_v0;
    s32 phi_v1;

    if (!(this->unk_4C2 & 0x200)) {
        if (this->unk_4C2 & 0x80) {
            phi_v1 = 1;
        } else {
            phi_v1 = 0;
        }
        phi_v0 = 1;
    } else {
        phi_v1 = 0;
        phi_v0 = 0;
    }

    if (limbIndex == 16) {
        func_8013AD9C(this->unk_4DC + 0x4000, this->unk_4DE + this->actor.shape.rot.y + 0x4000, &this->unk_36C,
                      &this->unk_378, phi_v0, phi_v1);
        Matrix_StatePop();
        Matrix_InsertTranslation(this->unk_36C.x, this->unk_36C.y, this->unk_36C.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateY(this->unk_378.y, MTXMODE_APPLY);
        Matrix_InsertXRotation_s(this->unk_378.x, MTXMODE_APPLY);
        Matrix_InsertZRotation_s(this->unk_378.z, MTXMODE_APPLY);
        Matrix_StatePush();
    }
}

void EnAl_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnAl* this = THIS;
    s32 i;

    if (this->unk_35C != 0) {
        OPEN_DISPS(globalCtx->state.gfxCtx);

        func_8012C28C(globalCtx->state.gfxCtx);
        Matrix_InsertTranslation(0.0f, 0.0f, 850.0f, MTXMODE_APPLY);
        SkelAnime_DrawTransformFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnAl_OverrideLimbDraw, EnAl_PostLimbDraw,
                                       EnAl_TransformLimbDraw, &this->actor);

        for (i = 0; i < ARRAY_COUNT(this->unk_190); i++) {
            Matrix_SetCurrentState(&this->unk_190[i]);

            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, D_80BE007C[i]);
        }

        CLOSE_DISPS(globalCtx->state.gfxCtx);
    }
}
