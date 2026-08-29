#pragma once

// Main objects
constexpr CLASS_ID CLSID_OBJECT_ACTOR = make_clsid("O_ACTOR ");
constexpr CLASS_ID CLSID_OBJECT_HLAMP = make_clsid("O_HLAMP ");
constexpr CLASS_ID CLSID_ENTITY = make_clsid("ENTITY  ");
constexpr CLASS_ID CLSID_SPECTATOR = make_clsid("SPECT   ");
constexpr CLASS_ID CLSID_OBJECT_PROJECTOR = make_clsid("O_SEARCH");

// all the monsters
constexpr CLASS_ID CLSID_LEVEL_POINT = make_clsid("LVLPOINT");
constexpr CLASS_ID CLSID_SCRIPT_OBJECT = make_clsid("SCRPTOBJ");
constexpr CLASS_ID CLSID_AI_GRAPH = make_clsid("AI_GRAPH");
constexpr CLASS_ID CLSID_AI_CROW = make_clsid("AI_CROW ");
constexpr CLASS_ID CLSID_AI_ZOMBIE = make_clsid("AI_ZOM  ");
constexpr CLASS_ID CLSID_AI_POLTERGEIST = make_clsid("AI_POLTR");

constexpr CLASS_ID CLSID_AI_FLESH = make_clsid("AI_FLESH");
constexpr CLASS_ID CLSID_AI_FLESH_GROUP = make_clsid("AI_FLE_G");

constexpr CLASS_ID CLSID_AI_PHANTOM = make_clsid("AI_PHANT");
constexpr CLASS_ID CLSID_AI_SPONGER = make_clsid("AI_SPONG");
constexpr CLASS_ID CLSID_AI_CONTROLLER = make_clsid("AI_CONTR");
constexpr CLASS_ID CLSID_AI_BLOODSUCKER = make_clsid("AI_BLOOD");
constexpr CLASS_ID CLSID_AI_STALKER = make_clsid("AI_STL  ");
constexpr CLASS_ID CLSID_AI_BURER = make_clsid("AI_BURER");
constexpr CLASS_ID CLSID_AI_GIANT = make_clsid("AI_GIANT");
constexpr CLASS_ID CLSID_AI_CHIMERA = make_clsid("AI_HIMER");
constexpr CLASS_ID CLSID_AI_FRACTURE = make_clsid("AI_FRACT");
constexpr CLASS_ID CLSID_AI_DOG_BLACK = make_clsid("AI_DOG_B");
constexpr CLASS_ID CLSID_AI_DOG_RED = make_clsid("AI_DOG_R");
constexpr CLASS_ID CLSID_AI_DOG_PSY = make_clsid("AI_DOG_P");
constexpr CLASS_ID CLSID_AI_DOG_PSY_PHANTOM = make_clsid("AI_DOG_F");
constexpr CLASS_ID CLSID_AI_TRADER = make_clsid("AI_TRADE");
constexpr CLASS_ID CLSID_AI_BOAR = make_clsid("AI_BOAR ");
constexpr CLASS_ID CLSID_AI_SNORK = make_clsid("AI_SNORK");
constexpr CLASS_ID CLSID_AI_CAT = make_clsid("AI_CAT  ");
constexpr CLASS_ID CLSID_AI_TUSHKANO = make_clsid("AI_TUSH ");
constexpr CLASS_ID CLSID_AI_RAT = make_clsid("AI_RAT  ");

// vehicles
constexpr CLASS_ID CLSID_CAR = make_clsid("C_NIVA  ");
constexpr CLASS_ID CLSID_VEHICLE_HELICOPTER = make_clsid("C_HLCPT ");
constexpr CLASS_ID CLSID_EVENT = make_clsid("EVENT   ");
constexpr CLASS_ID CLSID_OBJECT_FLYER = make_clsid("O_FLYER ");
constexpr CLASS_ID CLSID_OBJECT_DOOR = make_clsid("O_DOOR  ");
constexpr CLASS_ID CLSID_OBJECT_LIFT = make_clsid("O_LIFT  ");

// Artefacts
constexpr CLASS_ID CLSID_AF_MERCURY_BALL = make_clsid("AF_MBALL");
constexpr CLASS_ID CLSID_AF_GRAVI = make_clsid("AF_GRAVI");
constexpr CLASS_ID CLSID_AF_BLACKDROPS = make_clsid("AF_BDROP");
constexpr CLASS_ID CLSID_AF_NEEDLES = make_clsid("AF_NEEDL");
constexpr CLASS_ID CLSID_AF_BAST = make_clsid("AF_BAST ");
constexpr CLASS_ID CLSID_AF_BLACK_GRAVI = make_clsid("AF_BGRAV");
constexpr CLASS_ID CLSID_AF_DUMMY = make_clsid("AF_DUMMY");
constexpr CLASS_ID CLSID_AF_ZUDA = make_clsid("AF_ZUDA ");
constexpr CLASS_ID CLSID_AF_THORN = make_clsid("AF_THORN");
constexpr CLASS_ID CLSID_AF_FADED_BALL = make_clsid("AF_FBALL");
constexpr CLASS_ID CLSID_AF_ELECTRIC_BALL = make_clsid("AF_EBALL");
constexpr CLASS_ID CLSID_AF_RUSTY_HAIR = make_clsid("AF_RHAIR");
constexpr CLASS_ID CLSID_AF_GALANTINE = make_clsid("AF_GALAN");

constexpr CLASS_ID CLSID_ARTEFACT = make_clsid("ARTEFACT");

// Weapons
constexpr CLASS_ID CLSID_OBJECT_W_M134 = make_clsid("W_M134  ");
constexpr CLASS_ID CLSID_OBJECT_W_FN2000 = make_clsid("W_FN2000");
constexpr CLASS_ID CLSID_OBJECT_W_AK74 = make_clsid("W_AK74  ");
constexpr CLASS_ID CLSID_OBJECT_W_LR300 = make_clsid("W_LR300 ");
constexpr CLASS_ID CLSID_OBJECT_W_HPSA = make_clsid("W_HPSA  ");
constexpr CLASS_ID CLSID_OBJECT_W_PM = make_clsid("W_PM    ");
constexpr CLASS_ID CLSID_OBJECT_W_FORT = make_clsid("W_FORT  ");
constexpr CLASS_ID CLSID_OBJECT_W_BINOCULAR = make_clsid("W_BINOC ");
constexpr CLASS_ID CLSID_OBJECT_W_SHOTGUN = make_clsid("W_SHOTGN");
constexpr CLASS_ID CLSID_OBJECT_W_ASHOTGUN = make_clsid("W_ASHTGN");
//  [8/15/2006]
constexpr CLASS_ID CLSID_OBJECT_W_MAGAZINED = make_clsid("W_WMAGAZ");
//  [8/15/2006]
//  [8/17/2006]
constexpr CLASS_ID CLSID_OBJECT_W_MAGAZWGL = make_clsid("W_WMAGGL");
//  [8/17/2006]
constexpr CLASS_ID CLSID_OBJECT_W_SVD = make_clsid("W_SVD   ");
constexpr CLASS_ID CLSID_OBJECT_W_SVU = make_clsid("W_SVU   ");
constexpr CLASS_ID CLSID_OBJECT_W_RPG7 = make_clsid("W_RPG7  ");
constexpr CLASS_ID CLSID_OBJECT_W_VAL = make_clsid("W_VAL   ");
constexpr CLASS_ID CLSID_OBJECT_W_VINTOREZ = make_clsid("W_VINT  ");
constexpr CLASS_ID CLSID_OBJECT_W_WALTHER = make_clsid("W_WALTHR");
constexpr CLASS_ID CLSID_OBJECT_W_USP45 = make_clsid("W_USP45 ");
constexpr CLASS_ID CLSID_OBJECT_W_GROZA = make_clsid("W_GROZA ");
constexpr CLASS_ID CLSID_OBJECT_W_KNIFE = make_clsid("W_KNIFE ");
constexpr CLASS_ID CLSID_OBJECT_W_BM16 = make_clsid("W_BM16  ");
constexpr CLASS_ID CLSID_OBJECT_W_RG6 = make_clsid("W_RG6   ");

constexpr CLASS_ID CLSID_OBJECT_W_STATMGUN = make_clsid("W_STMGUN");
// Weapons Ammo
constexpr CLASS_ID CLSID_OBJECT_AMMO = make_clsid("AMMO    ");
//-----------------------------------------------------------------------------
constexpr CLASS_ID CLSID_OBJECT_A_VOG25 = make_clsid("A_VOG25 ");
constexpr CLASS_ID CLSID_OBJECT_A_OG7B = make_clsid("A_OG7B  ");
constexpr CLASS_ID CLSID_OBJECT_A_M209 = make_clsid("A_M209  ");
//-----------------------------------------------------------------------------
// Weapons Add-ons
constexpr CLASS_ID CLSID_OBJECT_W_SCOPE = make_clsid("W_SCOPE ");
constexpr CLASS_ID CLSID_OBJECT_W_SILENCER = make_clsid("W_SILENC");
constexpr CLASS_ID CLSID_OBJECT_W_GLAUNCHER = make_clsid("W_GLAUNC");

// Modifiers
constexpr CLASS_ID CLSID_OBJECT_M_QDAMAGE = make_clsid("O_QDMG  ");
constexpr CLASS_ID CLSID_OBJECT_M_IMMORTAL = make_clsid("O_IMMORT");
constexpr CLASS_ID CLSID_OBJECT_M_INVIS = make_clsid("O_INVIS ");

constexpr CLASS_ID CLSID_OBJECT_HEALTH = make_clsid("O_HEALTH");
constexpr CLASS_ID CLSID_OBJECT_ARMOR = make_clsid("O_ARMOR ");

constexpr CLASS_ID CLSID_OBJECT_TRIGGER = make_clsid("O_TRIGER");

// Targets
constexpr CLASS_ID CLSID_TARGET = make_clsid("T_BASE  ");
constexpr CLASS_ID CLSID_TARGET_ASSAULT = make_clsid("T_ASS   ");
constexpr CLASS_ID CLSID_TARGET_CS_BASE = make_clsid("T_CSBAS ");
constexpr CLASS_ID CLSID_TARGET_CS = make_clsid("T_CS    ");
constexpr CLASS_ID CLSID_TARGET_CS_CASK = make_clsid("T_CSCASK");

// Standard level object
constexpr CLASS_ID CLSID_OBJECT_ITEM_STD = make_clsid("O_ITEM  ");
constexpr CLASS_ID CLSID_OBJECT_BREAKABLE = make_clsid("O_BRKBL ");
constexpr CLASS_ID CLSID_OBJECT_CLIMABLE = make_clsid("O_CLMBL ");

//
constexpr CLASS_ID CLSID_PH_SKELETON_OBJECT = make_clsid("P_SKELET");
constexpr CLASS_ID CLSID_OBJECT_PHYSIC = make_clsid("O_PHYSIC");
constexpr CLASS_ID CLSID_PHYSICS_DESTROYABLE = make_clsid("P_DSTRBL");
constexpr CLASS_ID CLSID_INVENTORY_BOX = make_clsid("O_INVBOX");

// Zones
constexpr CLASS_ID CLSID_ZONE = make_clsid("Z_ZONE  ");
constexpr CLASS_ID CLSID_Z_MBALD = make_clsid("Z_MBALD ");
constexpr CLASS_ID CLSID_Z_MINCER = make_clsid("Z_MINCER");
constexpr CLASS_ID CLSID_Z_ACIDF = make_clsid("Z_ACIDF ");
constexpr CLASS_ID CLSID_Z_GALANT = make_clsid("Z_GALANT");
constexpr CLASS_ID CLSID_Z_RADIO = make_clsid("Z_RADIO ");
constexpr CLASS_ID CLSID_Z_BFUZZ = make_clsid("Z_BFUZZ ");
constexpr CLASS_ID CLSID_Z_RUSTYH = make_clsid("Z_RUSTYH");
constexpr CLASS_ID CLSID_Z_AMEBA = make_clsid("Z_AMEBA ");
constexpr CLASS_ID CLSID_Z_NOGRAVITY = make_clsid("Z_NOGRAV");
constexpr CLASS_ID CLSID_Z_FRYUP = make_clsid("Z_FRYUP ");
constexpr CLASS_ID CLSID_Z_DEAD = make_clsid("Z_DEAD  ");
constexpr CLASS_ID CLSID_LEVEL_CHANGER = make_clsid("LVLCHNGR");
constexpr CLASS_ID CLSID_SCRIPT_ZONE = make_clsid("SCRIPTZN");
constexpr CLASS_ID CLSID_Z_TEAM_BASE = make_clsid("Z_TEAMBS");
constexpr CLASS_ID CLSID_Z_TORRID = make_clsid("Z_TORRID");
constexpr CLASS_ID CLSID_SPACE_RESTRICTOR = make_clsid("SPACE_RS");
constexpr CLASS_ID CLSID_SMART_ZONE = make_clsid("SMRTZONE");
constexpr CLASS_ID CLSID_Z_CAMPFIRE = make_clsid("Z_CFIRE ");

// Detectors
constexpr CLASS_ID CLSID_DETECTOR_SIMPLE = make_clsid("D_SIMDET");
constexpr CLASS_ID CLSID_DETECTOR_ADVANCED = make_clsid("D_ADVANC");
constexpr CLASS_ID CLSID_DETECTOR_ELITE = make_clsid("D_ELITE ");
constexpr CLASS_ID CLSID_DETECTOR_SCIENTIFIC = make_clsid("D_SCIENT");

// PDA
constexpr CLASS_ID CLSID_DEVICE_PDA = make_clsid("D_PDA   ");

// Devices
constexpr CLASS_ID CLSID_DEVICE_TORCH = make_clsid("D_TORCH ");
constexpr CLASS_ID CLSID_DEVICE_AF_MERGER = make_clsid("D_AFMERG");
constexpr CLASS_ID CLSID_DEVICE_FLARE = make_clsid("D_FLARE ");

// Inventory items
constexpr CLASS_ID CLSID_IITEM_BOLT = make_clsid("II_BOLT ");

constexpr CLASS_ID CLSID_IITEM_MEDKIT = make_clsid("II_MEDKI");
constexpr CLASS_ID CLSID_IITEM_BANDAGE = make_clsid("II_BANDG");
constexpr CLASS_ID CLSID_IITEM_FOOD = make_clsid("II_FOOD ");
constexpr CLASS_ID CLSID_IITEM_BOTTLE = make_clsid("II_BOTTL");
constexpr CLASS_ID CLSID_IITEM_ANTIRAD = make_clsid("II_ANTIR");
constexpr CLASS_ID CLSID_IITEM_EXPLOSIVE = make_clsid("II_EXPLO");

// Info Document
constexpr CLASS_ID CLSID_IITEM_DOCUMENT = make_clsid("II_DOC  ");

constexpr CLASS_ID CLSID_IITEM_ATTACH = make_clsid("II_ATTCH");

// Grenades
constexpr CLASS_ID CLSID_GRENADE_F1 = make_clsid("G_F1    ");
constexpr CLASS_ID CLSID_OBJECT_G_RPG7 = make_clsid("G_RPG7  ");
constexpr CLASS_ID CLSID_GRENADE_RGD5 = make_clsid("G_RGD5  ");
constexpr CLASS_ID CLSID_OBJECT_G_FAKE = make_clsid("G_FAKE  ");

//---------------------------------------------------------------------------------
constexpr CLASS_ID CLSID_OBJECT_PLAYERS_BAG = make_clsid("MP_PLBAG");
//---------------------------------------------------------------------------------

// Equipment
constexpr CLASS_ID CLSID_EQUIPMENT_SIMPLE = make_clsid("EQU_SMPL");
constexpr CLASS_ID CLSID_EQUIPMENT_SCIENTIFIC = make_clsid("EQU_SCIE");
constexpr CLASS_ID CLSID_EQUIPMENT_STALKER = make_clsid("EQU_STLK");
constexpr CLASS_ID CLSID_EQUIPMENT_MILITARY = make_clsid("EQU_MLTR");
constexpr CLASS_ID CLSID_EQUIPMENT_EXO = make_clsid("EQU_EXO ");
constexpr CLASS_ID CLSID_EQUIPMENT_HELMET = make_clsid("EQ_HLMET");

// Game types
constexpr CLASS_ID CLSID_SV_GAME_SINGLE = make_clsid("SV_SINGL");
constexpr CLASS_ID CLSID_CL_GAME_SINGLE = make_clsid("CL_SINGL");

// Game UI types
constexpr CLASS_ID CLSID_GAME_UI_SINGLE = make_clsid("UI_SINGL");

constexpr CLASS_ID CLSID_ONLINE_OFFLINE_GROUP = make_clsid("ON_OFF_G");