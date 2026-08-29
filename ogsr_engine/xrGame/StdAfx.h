#pragma once

#pragma warning(disable : 4995)
#include "../xr_3da/stdafx.h"
#pragma warning(default : 4995)
#pragma warning(4 : 4018)
#pragma warning(4 : 4244)
#pragma warning(disable : 4505)

#include "..\xr_3da\ai_script_space.h"

#define THROW VERIFY
#define THROW2 VERIFY2
#define THROW3 VERIFY3

#include "../xr_3da/GameFont.h"
#include "../xr_3da/xr_object.h"
#include "../xr_3da/IGame_Level.h"
#include "../xr_3da/bone.h"
#include "../xrPhysics/xrPhysics.h"

#include "../xrCore/buffer_vector.h"

//////////////////////////////////////////////////////////////
#include "../xrNetServer/NET_Shared.h"
//////////////////////////////////////////////////////////////

#define D3DCOLOR_RGBA(r, g, b, a) (color_argb(a, r, g, b))
#define D3DCOLOR_XRGB(r, g, b) (color_argb(0xff, r, g, b))