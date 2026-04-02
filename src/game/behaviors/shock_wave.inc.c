// shock_wave.c.inc
extern s32 interact_shock(struct MarioState *s, u32 interactType, struct Object *o);
f32 D_8032F420[] = { 1.9f, 2.4f, 4.0f, 4.8f };

void bhv_bowser_shock_wave_init(void) {
    o->oOpacity = 255;
    o->oBowserShockWaveUnkF4 = 1.0f;

    struct Object *bowser = cur_obj_nearest_object_with_behavior(bhvBowser);
    if (bowser != NULL) {
        o->oPosX = bowser->oPosX;
        o->oPosZ = bowser->oPosZ;
        o->oPosY = bowser->oPosY;
        o->parentObj = bowser;
    }

    o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloor);
    o->oPosY = o->oFloorHeight + 5.0f; 
}
void bhv_bowser_shock_wave_loop(void) {
    f32 sp2C, sp28, sp24, sp20;
    s16 sp1E = 60;  // <--- LOWERING THIS (formerly 70). Controls how long the wave lasts.  

    if (o->oTimer < 2 && o->parentObj != o) {
        o->oPosX = o->parentObj->oPosX;
        o->oPosZ = o->parentObj->oPosZ;
        o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloor);
        o->oPosY = o->oFloorHeight + 5.0f;
    }
    
// CHANGE THE 10.0f TO A LOWER VALUE SO THAT IT GROWS SLOWER
    o->oBowserShockWaveUnkF4 = o->oTimer * 10.0f;
    obj_scale_xyz(o, o->oBowserShockWaveUnkF4, 1.0f, o->oBowserShockWaveUnkF4);
    
    if (gGlobalTimer % 3 == 0) o->oOpacity -= 1;
    if (o->oTimer > sp1E) o->oOpacity -= 5;
    
    if (o->oOpacity <= 0) {
        obj_mark_for_deletion(o);
        return;
    }

    f32 dx = o->oPosX - gMarioObject->oPosX;
    f32 dz = o->oPosZ - gMarioObject->oPosZ;
    f32 dist2D = sqrtf(dx*dx + dz*dz);

    
    if (o->oTimer < sp1E) {
        sp2C = o->oBowserShockWaveUnkF4 * D_8032F420[0];
        sp28 = o->oBowserShockWaveUnkF4 * D_8032F420[1];
        sp24 = o->oBowserShockWaveUnkF4 * D_8032F420[2];
        sp20 = o->oBowserShockWaveUnkF4 * D_8032F420[3];
        
        if ((dist2D > sp2C && dist2D < sp28) || (dist2D > sp24 && dist2D < sp20)) {
            f32 diffY = gMarioObject->oPosY - o->oPosY;
            if (diffY > -50.0f && diffY < 100.0f) {
                if (!(gMarioState->action & ACT_FLAG_INVULNERABLE)) {
                    interact_shock(gMarioState, INTERACT_SHOCK, o);
                 }
            }
        }
    }
}
