/*
 * File: z_sub_s.c
 * Description: Various miscellaneous helpers
 */

#include "global.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"

s16 sPathDayFlags[] = { 0x40, 0x20, 0x10, 8, 4, 2, 1, 0 };

#include "code/sub_s/sub_s.c"

Vec3f gOneVec3f = { 1.0f, 1.0f, 1.0f };

s32 D_801C5DBC[] = { 0, 1 }; // Unused

/**
 * Finds the first EnDoor instance with unk_1A4 == 5 and the specified switchFlag.
 */
EnDoor* SubS_FindDoor(GlobalContext* globalCtx, s32 switchFlag) {
    Actor* actor = NULL;
    EnDoor* door;

    while (true) {
        actor = SubS_FindActor(globalCtx, actor, ACTORCAT_DOOR, ACTOR_EN_DOOR);
        door = (EnDoor*)actor;

        if (actor == NULL) {
            break;
        }

        if ((door->unk_1A4 == 5) && (door->switchFlag == (u8)switchFlag)) {
            break;
        }

        if (actor->next == NULL) {
            door = NULL;
            break;
        }

        actor = actor->next;
    }

    return door;
}

Gfx* SubS_DrawTransformFlexLimb(GlobalContext* globalCtx, s32 limbIndex, void** skeleton, Vec3s* jointTable,
                                OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw,
                                TransformLimbDraw transformLimbDraw, Actor* actor, Mtx** mtx, Gfx* gfx) {
    StandardLimb* limb;
    Gfx* newDList;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;

    Matrix_StatePush();
    limb = Lib_SegmentedToVirtual(skeleton[limbIndex]);
    limbIndex++;
    rot = jointTable[limbIndex];
    pos.x = limb->jointPos.x;
    pos.y = limb->jointPos.y;
    pos.z = limb->jointPos.z;
    newDList = limbDList = limb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, limbIndex, &newDList, &pos, &rot, actor, &gfx)) {
        Matrix_JointPosition(&pos, &rot);
        Matrix_StatePush();

        transformLimbDraw(globalCtx, limbIndex, actor, &gfx);

        if (newDList != NULL) {
            Matrix_ToMtx(*mtx);
            gSPMatrix(gfx++, *mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(gfx++, newDList);
            (*mtx)++;
        } else if (limbDList != NULL) {
            Matrix_ToMtx(*mtx);
            (*mtx)++;
        }
        Matrix_StatePop();
    }
    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, limbIndex, &limbDList, &rot, actor, &gfx);
    }
    if (limb->child != LIMB_DONE) {
        gfx = SubS_DrawTransformFlexLimb(globalCtx, limb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw,
                                         transformLimbDraw, actor, mtx, gfx);
    }
    Matrix_StatePop();
    if (limb->sibling != LIMB_DONE) {
        gfx = SubS_DrawTransformFlexLimb(globalCtx, limb->sibling, skeleton, jointTable, overrideLimbDraw, postLimbDraw,
                                         transformLimbDraw, actor, mtx, gfx);
    }
    return gfx;
}

/**
 * Draw all limbs of type `StandardLimb` in a given flexible skeleton
 * Limbs in a flexible skeleton have meshes that can stretch to line up with other limbs.
 * An array of matrices is dynamically allocated so each limb can access any transform to ensure its meshes line up.
 *
 * Also makes use of a `TransformLimbDraw`, which transforms limbs based on world coordinates, as opposed to local limb
 * coordinates.
 * Note that the `TransformLimbDraw` does not have a NULL check, so must be provided even if empty.
 */
Gfx* SubS_DrawTransformFlex(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, s32 dListCount,
                            OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw,
                            TransformLimbDraw transformLimbDraw, Actor* actor, Gfx* gfx) {
    StandardLimb* rootLimb;
    s32 pad;
    Gfx* newDlist;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;
    Mtx* mtx = GRAPH_ALLOC(globalCtx->state.gfxCtx, ALIGN16(dListCount * sizeof(Mtx)));

    if (skeleton == NULL) {
        return NULL;
    }

    gSPSegment(gfx++, 0x0D, mtx);
    Matrix_StatePush();
    rootLimb = Lib_SegmentedToVirtual(skeleton[0]);
    pos.x = jointTable->x;
    pos.y = jointTable->y;
    pos.z = jointTable->z;
    rot = jointTable[1];
    newDlist = rootLimb->dList;
    limbDList = rootLimb->dList;

    if (overrideLimbDraw == NULL || !overrideLimbDraw(globalCtx, 1, &newDlist, &pos, &rot, actor, &gfx)) {
        Matrix_JointPosition(&pos, &rot);
        Matrix_StatePush();

        transformLimbDraw(globalCtx, 1, actor, &gfx);

        if (newDlist != NULL) {
            Matrix_ToMtx(mtx);
            gSPMatrix(gfx++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(gfx++, newDlist);
            mtx++;
        } else if (limbDList != NULL) {
            Matrix_ToMtx(mtx);
            mtx++;
        }
        Matrix_StatePop();
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, 1, &limbDList, &rot, actor, &gfx);
    }

    if (rootLimb->child != LIMB_DONE) {
        gfx = SubS_DrawTransformFlexLimb(globalCtx, rootLimb->child, skeleton, jointTable, overrideLimbDraw,
                                         postLimbDraw, transformLimbDraw, actor, &mtx, gfx);
    }
    Matrix_StatePop();
    return gfx;
}

s32 SubS_InCsMode(GlobalContext* globalCtx) {
    s32 inCsMode = false;

    if (Play_InCsMode(globalCtx)) {
        inCsMode = true;
    }

    return inCsMode;
}

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013AD9C.s")

void SubS_UpdateFlags(u16* flags, u16 setBits, u16 unsetBits) {
    *flags = (*flags & ~unsetBits) | setBits;
}

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013AF00.s")

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013B010.s")

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013B0C8.s")

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013B350.s")

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013B6B0.s")

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013B878.s")

Path* SubS_GetAdditionalPath(GlobalContext* globalCtx, u8 pathIndex, s32 max) {
    Path* path;
    s32 i = 0;

    do {
        path = &globalCtx->setupPathList[pathIndex];
        if (i >= max) {
            break;
        }
        pathIndex = path->unk1;
        i++;
    } while (pathIndex != 0xFF);

    return path;
}

/**
 * Finds the nearest actor instance of a specified Id and category to an actor.
 */
Actor* SubS_FindNearestActor(Actor* actor, GlobalContext* globalCtx, u8 actorCategory, s16 actorId) {
    Actor* actorIter = NULL;
    Actor* actorTmp;
    f32 dist;
    Actor* closestActor = NULL;
    f32 minDist = 99999.0f;
    s32 isSetup = false;

    do {
        actorIter = SubS_FindActor(globalCtx, actorIter, actorCategory, actorId);

        actorTmp = actorIter;
        if (actorTmp == NULL) {
            break;
        }
        actorIter = actorTmp;

        if (actorIter != actor) {
            dist = Actor_DistanceBetweenActors(actor, actorIter);
            if (!isSetup || dist < minDist) {
                closestActor = actorIter;
                minDist = dist;
                isSetup = true;
            }
        }

        actorIter = actorIter->next;
    } while (actorIter != NULL);

    return closestActor;
}

s32 SubS_ChangeAnimationByInfoS(SkelAnime* skelAnime, AnimationInfoS* animations, s32 index) {
    s32 endFrame;
    s32 startFrame;

    animations += index;
    endFrame = animations->frameCount;
    if (animations->frameCount < 0) {
        endFrame = Animation_GetLastFrame(&animations->animation->common);
    }
    startFrame = animations->startFrame;
    if (startFrame >= endFrame || startFrame < 0) {
        return false;
    }
    if (animations->playSpeed < 0.0f) {
        SWAP(s32, endFrame, startFrame);
    }
    Animation_Change(skelAnime, animations->animation, animations->playSpeed, startFrame, endFrame, animations->mode,
                     animations->morphFrames);
    return true;
}

s32 SubS_HasReachedPoint(Actor* actor, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == count - 1) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    func_8017B7F8(&point, RADF_TO_BINANG(func_80086B30(diffX, diffZ)), &px, &pz, &d);
    if (((px * actor->world.pos.x) + (pz * actor->world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

Path* SubS_GetDayDependentPath(GlobalContext* globalCtx, u8 pathIndex, u8 max, s32* startPointIndex) {
    Path* path = NULL;
    s32 found = false;
    s32 time = (((s16)TIME_TO_MINUTES_F(gSaveContext.save.time) % 60) +
                ((s16)TIME_TO_MINUTES_F(gSaveContext.save.time) / 60) * 60) /
               30;
    s32 day = CURRENT_DAY;

    if (pathIndex == max) {
        return NULL;
    }

    while (pathIndex != 0xFF) {
        path = &globalCtx->setupPathList[pathIndex];
        if (sPathDayFlags[day] & path->unk2) {
            found = true;
            break;
        }
        pathIndex = path->unk1;
    }

    if (found == true) {
        *startPointIndex = time;
        *startPointIndex = CLAMP(*startPointIndex, 0, path->count - 1);
    } else {
        *startPointIndex = 0;
    }

    return path;
}

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013C068.s")

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013C624.s")

s32 SubS_CopyPointFromPathCheckBounds(Path* path, s32 pointIndex, Vec3f* dst) {
    Vec3s* point;

    if ((path == NULL) || (pointIndex >= path->count) || (pointIndex < 0)) {
        return false;
    }

    point = Lib_SegmentedToVirtual(path->points);
    point = &point[pointIndex];
    dst->x = point->x;
    dst->y = point->y;
    dst->z = point->z;
    return true;
}

//! TODO: Needs docs with func_800B8500
s32 func_8013C964(Actor* actor, GlobalContext* globalCtx, f32 xzRange, f32 yRange, s32 itemId, s32 type) {
    s32 ret = false;
    s16 x;
    s16 y;
    f32 xzDistToPlayerTemp;

    Actor_GetScreenPos(globalCtx, actor, &x, &y);

    switch (type) {
        case 1:
            yRange = fabsf(actor->playerHeightRel) + 1.0f;
            xzRange = actor->xzDistToPlayer + 1.0f;
            ret = Actor_PickUp(actor, globalCtx, itemId, xzRange, yRange);
            break;
        case 2:
            if ((fabsf(actor->playerHeightRel) <= yRange) && (actor->xzDistToPlayer <= xzRange)) {
                ret = func_800B8500(actor, globalCtx, xzRange, yRange, itemId);
            }
            break;
        case 3:
            //! @bug: Both x and y conditionals are always true, || should be an &&
            if (((x >= 0) || (x < SCREEN_WIDTH)) && ((y >= 0) || (y < SCREEN_HEIGHT))) {
                ret = func_800B8500(actor, globalCtx, xzRange, yRange, itemId);
            }
            break;
        case 4:
            yRange = fabsf(actor->playerHeightRel) + 1.0f;
            xzRange = actor->xzDistToPlayer + 1.0f;
            xzDistToPlayerTemp = actor->xzDistToPlayer;
            actor->xzDistToPlayer = 0.0f;
            actor->flags |= 0x10000;
            ret = func_800B8500(actor, globalCtx, xzRange, yRange, itemId);
            actor->xzDistToPlayer = xzDistToPlayerTemp;
            break;
        case 5:
            //! @bug: Both x and y conditionals are always true, || should be an &&
            if (((x >= 0) || (x < SCREEN_WIDTH)) && ((y >= 0) || (y < SCREEN_HEIGHT)) &&
                (fabsf(actor->playerHeightRel) <= yRange) && (actor->xzDistToPlayer <= xzRange) && actor->isTargeted) {
                actor->flags |= 0x10000;
                ret = func_800B8500(actor, globalCtx, xzRange, yRange, itemId);
            }
            break;
        case 6:
            //! @bug: Both x and y conditionals are always true, || should be an &&
            if (((x >= 0) || (x < SCREEN_WIDTH)) && ((y >= 0) || (y < SCREEN_HEIGHT)) &&
                (fabsf(actor->playerHeightRel) <= yRange) && (actor->xzDistToPlayer <= xzRange)) {
                actor->flags |= 0x10000;
                ret = func_800B8500(actor, globalCtx, xzRange, yRange, itemId);
            }
            break;
    }
    return ret;
}

const u8 sShadowMaps[4][12][12] = {
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
    },
};

void SubS_FillShadowTex(s32 startCol, s32 startRow, u8* tex, s32 size) {
    s32 i;
    s32 j;
    s32 start;

    for (i = 0; i < 12; i++) {
        start = ((startRow + i) * 64) + startCol - 390;
        for (j = 0; j < 12; j++) {
            if (sShadowMaps[size][i][j] != 0) {
                if ((start + j >= 0) && (start + j < SUBS_SHADOW_TEX_SIZE)) {
                    tex[start + j] = 255;
                }
            }
        }
    }
}

void SubS_GenShadowTex(Vec3f bodyPartsPos[], Vec3f* worldPos, u8* tex, f32 tween, u8 bodyPartsNum, u8 sizes[],
                       s8 parentBodyParts[]) {
    Vec3f pos;
    Vec3f startVec;
    s32 i;
    s32 parentBodyPart;
    Vec3f* bodyPartPos;
    s32 startCol;
    s32 startRow;

    for (i = 0; i < bodyPartsNum; i++) {
        if (parentBodyParts[i] >= 0) {
            parentBodyPart = parentBodyParts[i];
            bodyPartPos = &bodyPartsPos[i];

            pos.x = (bodyPartsPos[parentBodyPart].x - bodyPartPos->x) * tween + (bodyPartPos->x - worldPos->x);
            pos.y = (bodyPartsPos[parentBodyPart].y - bodyPartPos->y) * tween + (bodyPartPos->y - worldPos->y);
            pos.z = (bodyPartsPos[parentBodyPart].z - bodyPartPos->z) * tween + (bodyPartPos->z - worldPos->z);
        } else {
            bodyPartPos = &bodyPartsPos[i];

            pos.x = bodyPartPos->x - worldPos->x;
            pos.y = bodyPartPos->y - worldPos->y;
            pos.z = bodyPartPos->z - worldPos->z;
        }

        Matrix_MultiplyVector3fByState(&pos, &startVec);
        startCol = 64.0f + startVec.x;
        startRow = 64.0f - startVec.z;
        SubS_FillShadowTex(startCol >> 1, startRow >> 1, tex, sizes[i]);
    }
}

void SubS_DrawShadowTex(Actor* actor, GameState* gameState, u8* tex) {
    s32 pad;
    GraphicsContext* gfxCtx = gameState->gfxCtx;

    OPEN_DISPS(gfxCtx);

    func_8012C28C(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 100);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    Matrix_InsertTranslation(actor->world.pos.x, 0.0f, actor->world.pos.z, MTXMODE_NEW);
    Matrix_Scale(0.6f, 1.0f, 0.6f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gShadowDL);
    gDPLoadTextureBlock(POLY_OPA_DISP++, tex, G_IM_FMT_I, G_IM_SIZ_8b, SUBS_SHADOW_TEX_WIDTH, SUBS_SHADOW_TEX_HEIGHT, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, gShadowVtxDL);

    CLOSE_DISPS(gfxCtx);
}

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013D0E0.s")

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013D2E0.s")

s32 SubS_AngleDiffLessEqual(s16 angleA, s16 threshold, s16 angleB) {
    return (ABS_ALT(BINANG_SUB(angleB, angleA)) <= threshold) ? true : false;
}

Path* SubS_GetPathByIndex(GlobalContext* globalCtx, s16 pathIndex, s16 max) {
    return (pathIndex != max) ? &globalCtx->setupPathList[pathIndex] : NULL;
}

s32 SubS_CopyPointFromPath(Path* path, s32 pointIndex, Vec3f* dst) {
    Vec3s* point;

    if (path == NULL) {
        return false;
    }

    point = Lib_SegmentedToVirtual(path->points);
    point = &point[pointIndex];
    dst->x = point->x;
    dst->y = point->y;
    dst->z = point->z;

    return true;
}

s16 SubS_GetDistSqAndOrientPoints(Vec3f* vecA, Vec3f* vecB, f32* distSq) {
    f32 diffX = vecA->x - vecB->x;
    f32 diffZ = vecA->z - vecB->z;

    *distSq = SQ(diffX) + SQ(diffZ);
    return Math_Atan2S(diffX, diffZ);
}

/**
 * Returns true when the actor has reached the inputed point
 */
s32 SubS_MoveActorToPoint(Actor* actor, Vec3f* point, s16 rotStep) {
    Vec3f offsetBefore;
    Vec3f offsetAfter;
    f32 distSqBefore;
    f32 distSqAfter;

    Actor_OffsetOfPointInActorCoords(actor, &offsetBefore, point);
    Math_SmoothStepToS(&actor->world.rot.y, SubS_GetDistSqAndOrientPoints(point, &actor->world.pos, &distSqBefore), 4,
                       rotStep, 1);
    actor->shape.rot.y = actor->world.rot.y;
    Actor_MoveWithGravity(actor);
    Actor_OffsetOfPointInActorCoords(actor, &offsetAfter, point);
    SubS_GetDistSqAndOrientPoints(point, &actor->world.pos, &distSqAfter);
    return ((offsetBefore.z > 0.0f) && (offsetAfter.z <= 0.0f)) ? true : false;
}

s16 SubS_GetDistSqAndOrientPath(Path* path, s32 pointIndex, Vec3f* pos, f32* distSq) {
    Vec3s* point;
    f32 diffX = 0.0f;
    f32 diffZ = 0.0f;

    if (path != NULL) {
        point = Lib_SegmentedToVirtual(path->points);
        point = &point[pointIndex];
        diffX = point->x - pos->x;
        diffZ = point->z - pos->z;
    }

    *distSq = SQ(diffX) + SQ(diffZ);
    return Math_Atan2S(diffX, diffZ);
}

s8 SubS_IsObjectLoaded(s8 index, GlobalContext* globalCtx) {
    return !Object_IsLoaded(&globalCtx->objectCtx, index) ? false : true;
}

s8 SubS_GetObjectIndex(s16 id, GlobalContext* globalCtx) {
    return Object_GetIndex(&globalCtx->objectCtx, id);
}

/**
 * Finds the first actor instance of a specified Id and category.
 */
Actor* SubS_FindActor(GlobalContext* globalCtx, Actor* actorListStart, u8 actorCategory, s16 actorId) {
    Actor* actor = actorListStart;

    if (actor == NULL) {
        actor = globalCtx->actorCtx.actorLists[actorCategory].first;
    }

    while (actor != NULL && actorId != actor->id) {
        actor = actor->next;
    }

    return actor;
}

s32 SubS_FillLimbRotTables(GlobalContext* globalCtx, s16* limbRotTableY, s16* limbRotTableZ, s32 numLimbs) {
    s32 i;
    u32 frames = globalCtx->gameplayFrames;

    for (i = 0; i < numLimbs; i++) {
        limbRotTableY[i] = (i * 50 + 0x814) * frames;
        limbRotTableZ[i] = (i * 50 + 0x940) * frames;
    }

    return true;
}

s32 SubS_IsFloorAbove(GlobalContext* globalCtx, Vec3f* pos, f32 distAbove) {
    CollisionPoly* outPoly;
    Vec3f posA;
    Vec3f posB;
    Vec3f posResult;
    s32 bgId;

    posA = posB = *pos;
    posB.y += distAbove;
    return BgCheck_EntityLineTest1(&globalCtx->colCtx, &posA, &posB, &posResult, &outPoly, false, true, false, true,
                                   &bgId);
}

s32 SubS_CopyPointFromPathList(Path* paths, s32 pathIndex, s32 pointIndex, Vec3f* dst) {
    Path* path = &paths[pathIndex];
    Vec3s* point = &((Vec3s*)Lib_SegmentedToVirtual(path->points))[pointIndex];

    dst->x = point->x;
    dst->y = point->y;
    dst->z = point->z;

    return false;
}

u8 SubS_GetPathCount(Path* paths, s32 index) {
    Path* path = &paths[index];

    return path->count;
}

void SubS_ActorPathing_Init(GlobalContext* globalCtx, Vec3f* worldPos, Actor* actor, ActorPathing* actorPath,
                            Path* paths, s32 pathIndex, s32 begPointIndex, s32 endPointIndex, s32 curPointIndex,
                            u8 flags) {
    Path* path;

    actorPath->setupPathList = globalCtx->setupPathList;
    actorPath->pathIndex = pathIndex;
    path = &paths[pathIndex];
    actorPath->points = Lib_SegmentedToVirtual(path->points);
    actorPath->count = path->count;
    actorPath->begPointIndex = begPointIndex;
    if (endPointIndex == 0) {
        actorPath->endPointIndex = actorPath->count - 1;
    } else if (endPointIndex > 0) {
        actorPath->endPointIndex = endPointIndex;
    } else {
        //! @bug: endPointIndex is negative, subtraction causes result to be past the end
        actorPath->endPointIndex = (actorPath->count - endPointIndex) - 1;
    }
    actorPath->curPointIndex = curPointIndex;
    actorPath->curPoint.x = actorPath->points[0].x;
    actorPath->curPoint.y = actorPath->points[0].y;
    actorPath->curPoint.z = actorPath->points[0].z;
    Math_Vec3f_Copy(&actorPath->prevPoint, &actorPath->curPoint);
    actorPath->worldPos = worldPos;
    actorPath->actor = actor;
    actorPath->flags = flags;
    actorPath->prevFlags = flags;
}

s32 SubS_ActorPathing_Update(GlobalContext* globalCtx, ActorPathing* actorPath,
                             ActorPathingComputeFunc computePointInfoFunc, ActorPathingUpdateFunc updateActorInfoFunc,
                             ActorPathingUpdateFunc moveFunc, ActorPathingUpdateFunc setNextPointFunc) {
    s32 shouldSetNextPoint;
    s32 reupdate;

    actorPath->computePointInfoFunc = computePointInfoFunc;
    actorPath->updateActorInfoFunc = updateActorInfoFunc;
    actorPath->moveFunc = moveFunc;
    actorPath->setNextPointFunc = setNextPointFunc;
    actorPath->flags &= ~ACTOR_PATHING_REACHED_TEMPORARY;
    reupdate = false;
    if (actorPath->flags & ACTOR_PATHING_MOVE_BACKWARDS) {
        if (!(actorPath->prevFlags & ACTOR_PATHING_MOVE_BACKWARDS)) {
            actorPath->curPointIndex--;
        }
    } else if (actorPath->prevFlags & ACTOR_PATHING_MOVE_BACKWARDS) {
        actorPath->curPointIndex++;
    }
    do {
        shouldSetNextPoint = false;
        if (actorPath->computePointInfoFunc != NULL) {
            actorPath->computePointInfoFunc(globalCtx, actorPath);
        }
        if (actorPath->updateActorInfoFunc != NULL) {
            shouldSetNextPoint = actorPath->updateActorInfoFunc(globalCtx, actorPath);
        }
        if (shouldSetNextPoint) {
            if (actorPath->setNextPointFunc != NULL) {
                reupdate = actorPath->setNextPointFunc(globalCtx, actorPath);
            }
        } else if (actorPath->moveFunc != NULL) {
            reupdate = actorPath->moveFunc(globalCtx, actorPath);
        }
    } while (reupdate);
    actorPath->prevFlags = actorPath->flags;
    return false;
}

void SubS_ActorPathing_ComputePointInfo(GlobalContext* globalCtx, ActorPathing* actorPath) {
    Vec3f diff;

    actorPath->curPoint.x = actorPath->points[actorPath->curPointIndex].x + actorPath->pointOffset.x;
    actorPath->curPoint.y = actorPath->points[actorPath->curPointIndex].y + actorPath->pointOffset.y;
    actorPath->curPoint.z = actorPath->points[actorPath->curPointIndex].z + actorPath->pointOffset.z;
    diff.x = actorPath->curPoint.x - actorPath->worldPos->x;
    diff.y = actorPath->curPoint.y - actorPath->worldPos->y;
    diff.z = actorPath->curPoint.z - actorPath->worldPos->z;
    actorPath->distSqToCurPointXZ = Math3D_XZLengthSquared(diff.x, diff.z);
    actorPath->distSqToCurPoint = Math3D_LengthSquared(&diff);
    actorPath->rotToCurPoint.y = Math_FAtan2F(diff.z, diff.x);
    actorPath->rotToCurPoint.x = Math_FAtan2F(sqrtf(actorPath->distSqToCurPointXZ), -diff.y);
    actorPath->rotToCurPoint.z = 0;
}

s32 SubS_ActorPathing_MoveWithGravity(GlobalContext* globalCtx, ActorPathing* actorPath) {
    Actor_MoveWithGravity(actorPath->actor);
    return false;
}

s32 SubS_ActorPathing_MoveWithoutGravityReverse(GlobalContext* globalCtx, ActorPathing* actorPath) {
    Actor_MoveWithoutGravityReverse(actorPath->actor);
    return false;
}

s32 SubS_ActorPathing_SetNextPoint(GlobalContext* globalCtx, ActorPathing* actorPath) {
    s32 reupdate = true;

    Math_Vec3f_Copy(&actorPath->prevPoint, &actorPath->curPoint);
    if (!(actorPath->flags & ACTOR_PATHING_MOVE_BACKWARDS)) {
        if (actorPath->curPointIndex >= actorPath->endPointIndex) {
            if (actorPath->flags & ACTOR_PATHING_RETURN_TO_START) {
                actorPath->curPointIndex = actorPath->begPointIndex;
            } else if (actorPath->flags & ACTOR_PATHING_SWITCH_DIRECTION) {
                actorPath->flags |= ACTOR_PATHING_MOVE_BACKWARDS;
            } else {
                reupdate = false;
            }
            actorPath->flags |= ACTOR_PATHING_REACHED_END;
        } else {
            actorPath->curPointIndex++;
        }
        actorPath->flags |= ACTOR_PATHING_REACHED_POINT;
    } else {
        if (actorPath->begPointIndex >= actorPath->curPointIndex) {
            if (actorPath->flags & ACTOR_PATHING_RETURN_TO_START) {
                actorPath->curPointIndex = actorPath->endPointIndex;
            } else if (actorPath->flags & ACTOR_PATHING_SWITCH_DIRECTION) {
                actorPath->flags &= ~ACTOR_PATHING_MOVE_BACKWARDS;
            } else {
                reupdate = false;
            }
            actorPath->flags |= ACTOR_PATHING_REACHED_END;
        } else {
            actorPath->curPointIndex--;
        }
    }
    actorPath->flags |= ACTOR_PATHING_REACHED_POINT;
    return reupdate;
}

void SubS_ChangeAnimationBySpeedInfo(SkelAnime* skelAnime, AnimationSpeedInfo* animations, s32 nextIndex,
                                     s32* curIndex) {
    AnimationSpeedInfo* animation = &animations[nextIndex];
    f32 startFrame = skelAnime->curFrame;
    f32 endFrame;
    f32 morphFrames;

    if ((*curIndex < 0) || (nextIndex == *curIndex)) {
        morphFrames = 0.0f;
        if (*curIndex < 0) {
            startFrame = 0.0f;
        }
    } else {
        morphFrames = animation->morphFrames;
        if (nextIndex != *curIndex) {
            startFrame = 0.0f;
        }
    }
    if (animation->playSpeed >= 0.0f) {
        endFrame = Animation_GetLastFrame(&animation->animation->common);
    } else {
        startFrame = Animation_GetLastFrame(&animation->animation->common);
        endFrame = 0.0f;
    }
    Animation_Change(skelAnime, animation->animation, animation->playSpeed, startFrame, endFrame, animation->mode,
                     morphFrames);
    *curIndex = nextIndex;
}

s32 SubS_StartActorCutscene(Actor* actor, s16 nextCutscene, s16 curCutscene, s32 type) {
    s32 isStarted = false;

    if ((curCutscene != -1) && (ActorCutscene_GetCurrentIndex() == curCutscene)) {
        ActorCutscene_Stop(curCutscene);
        ActorCutscene_SetIntentToPlay(nextCutscene);
    } else if (ActorCutscene_GetCanPlayNext(nextCutscene)) {
        switch (type) {
            case SUBS_CUTSCENE_SET_UNK_LINK_FIELDS:
                ActorCutscene_StartAndSetUnkLinkFields(nextCutscene, actor);
                break;
            case SUBS_CUTSCENE_NORMAL:
                ActorCutscene_Start(nextCutscene, actor);
                break;
            case SUBS_CUTSCENE_SET_FLAG:
                ActorCutscene_StartAndSetFlag(nextCutscene, actor);
                break;
        }
        isStarted = true;
    } else {
        ActorCutscene_SetIntentToPlay(nextCutscene);
    }

    return isStarted;
}

s32 SubS_FillCutscenesList(Actor* actor, s16 cutscenes[], s16 numCutscenes) {
    s16 cs;
    s32 i;

    for (i = 0; i < numCutscenes; i++) {
        cutscenes[i] = -1;
    }

    cs = actor->cutscene;
    i = 0;

    while (cs != -1) {
        // Note: Infinite loop if numCutscenes is less than possible additional cutscenes
        if (i < numCutscenes) {
            cutscenes[i] = cs;
            cs = ActorCutscene_GetAdditionalCutscene(cs);
            i++;
        }
    }

    return i;
}

/**
 * Computes a plane based on a point on the plane, a unit vector and two angles
 *
 * @param[in] point a point on the plane
 * @param[in] unitVec the unit vector rotated that becomes the plane's normal
 * @param[in] rot the angles to rotate with, uses just the x and y components
 * @param[out] plane the computed plane
 *
 * Notes:
 *  The unit input vector is expected to already be normalized (only uses are with the z unit vector)
 *
 */
void SubS_ConstructPlane(Vec3f* point, Vec3f* unitVec, Vec3s* rot, Plane* plane) {
    f32 sin;
    f32 cos;
    f32 temp;
    f32 unitVecZ;
    f32 normY;
    f32 unitVecYX;

    sin = Math_SinS(-rot->x);
    cos = Math_CosS(-rot->x);
    unitVecZ = unitVec->z;
    unitVecYX = unitVec->y;

    // Apply a rotation by -x about the X axis
    temp = (unitVecZ * cos) - (unitVecYX * sin);
    normY = (unitVecZ * sin) + (unitVecYX * cos);

    sin = Math_SinS(rot->y);
    cos = Math_CosS(rot->y);
    unitVecYX = unitVec->x;
    plane->normal.y = normY;

    // Apply a rotation by y about the Y axis
    plane->normal.z = (temp * cos) - (unitVecYX * sin);
    plane->normal.x = (temp * sin) + (unitVecYX * cos);

    plane->originDist = -((point->x * plane->normal.x) + (plane->normal.y * point->y) + (plane->normal.z * point->z));
}

s32 SubS_LineSegVsPlane(Vec3f* point, Vec3s* rot, Vec3f* unitVec, Vec3f* linePointA, Vec3f* linePointB,
                        Vec3f* intersect) {
    s32 lineSegVsPlane;
    Plane plane;

    SubS_ConstructPlane(point, unitVec, rot, &plane);
    lineSegVsPlane = Math3D_LineSegVsPlane(plane.normal.x, plane.normal.y, plane.normal.z, plane.originDist, linePointA,
                                           linePointB, intersect, false);

    return lineSegVsPlane ? true : false;
}

/**
 * Finds the first actor instance of a specified Id and category verified with a custom callback.
 * The callback should return `true` when the actor is succesfully verified.
 */
Actor* SubS_FindActorCustom(GlobalContext* globalCtx, Actor* actor, Actor* actorListStart, u8 actorCategory,
                            s16 actorId, void* verifyData, VerifyActor verifyActor) {
    Actor* actorIter = actorListStart;

    if (actorListStart == NULL) {
        actorIter = globalCtx->actorCtx.actorLists[actorCategory].first;
    }

    while (actorIter != NULL && (actorId != actorIter->id ||
                                 (actorId == actorIter->id &&
                                  (verifyActor == NULL ||
                                   (verifyActor != NULL && !verifyActor(globalCtx, actor, actorIter, verifyData)))))) {
        actorIter = actorIter->next;
    }

    return actorIter;
}

//! TODO: Needs docs with func_800B8500
s32 func_8013E748(Actor* actor, GlobalContext* globalCtx, f32 xzRange, f32 yRange, s32 exchangeItemId, void* data,
                  func_8013E748_VerifyFunc verifyFunc) {
    s32 ret = false;

    if ((verifyFunc == NULL) || ((verifyFunc != NULL) && verifyFunc(globalCtx, actor, data))) {
        ret = func_800B8500(actor, globalCtx, xzRange, yRange, exchangeItemId);
    }
    return ret;
}

s32 SubS_ActorAndPlayerFaceEachOther(GlobalContext* globalCtx, Actor* actor, void* data) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3s* yawTols = (Vec3s*)data;
    s16 playerYaw = ABS(BINANG_SUB(Actor_YawBetweenActors(&player->actor, actor), player->actor.shape.rot.y));
    s16 actorYaw = ABS(BINANG_SUB(actor->yawTowardsPlayer, actor->shape.rot.y));
    s32 areFacing = false;
    s32 actorYawTol = ABS(yawTols->y);
    s32 playerYawTol;

    if (actorYaw < (s16)actorYawTol) {
        playerYawTol = ABS(yawTols->x);
        if (playerYaw < (s16)playerYawTol) {
            areFacing = true;
        }
    }

    return areFacing;
}

//! TODO: Needs docs with func_800B8500
s32 func_8013E8F8(Actor* actor, GlobalContext* globalCtx, f32 xzRange, f32 yRange, s32 exhangeItemId, s16 playerYawTol,
                  s16 actorYawTol) {
    Vec3s yawTols;

    yawTols.x = playerYawTol;
    yawTols.y = actorYawTol;
    return func_8013E748(actor, globalCtx, xzRange, yRange, exhangeItemId, &yawTols, SubS_ActorAndPlayerFaceEachOther);
}

#pragma GLOBAL_ASM("asm/non_matchings/code/z_sub_s/func_8013E950.s")
