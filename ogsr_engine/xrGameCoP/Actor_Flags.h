#pragma once

enum
{
    AF_GODMODE = (1 << 0),
    AF_NO_CLIP = (1 << 1),
    AF_UNLIMITEDAMMO = (1 << 2),
    AF_RUN_BACKWARD = (1 << 3),
    AF_AUTOPICKUP = (1 << 4),
    AF_PSP = (1 << 5),
    AF_DYNAMIC_MUSIC = (1 << 6),
    AF_GODMODE_RT = (1 << 7),
    AF_IMPORTANT_SAVE = (1 << 8),
    AF_CROUCH_TOGGLE = (1 << 9),
    AF_LOADING_STAGES = (1 << 10),
    AF_CROSSHAIR_DBG = (1 << 11),
    AF_DOF_RELOAD = (1 << 12),
    AF_DOF_ZOOM = (1 << 13),
    AF_ZONES_DBG = (1 << 14),
    AF_VERTEX_DBG = (1 << 15),
    AF_WPN_AIM_TOGGLE = (1 << 16),
    AF_MUSIC_TRACKS = (1 << 17),
    AF_AMMO_ON_BELT = (1 << 18),
    AF_3D_SCOPES = (1 << 19),
    AF_CAM_COLLISION = 1 << 20,
    AF_MOUSE_WHEEL_SWITCH_SLOTS = 1 << 21,
    // AF_3D_PDA = 1 << 22,
    AF_AI_VOLUMETRIC_LIGHTS = 1 << 23,
    AF_EFFECTS_ON_DEMORECORD = 1 << 24,
    AF_FIRST_PERSON_DEATH = 1 << 25,
    AF_LOCK_RELOAD = 1 << 26,
    AF_WEAPON_BOBBING = 1 << 27,
    AF_ACTOR_SHADOW = 1 << 28,
    AF_THROW_DEBUG = 1 << 29,
};

extern Flags32 psActorFlags;
extern BOOL GodMode();

extern int psActorSleepTime;
