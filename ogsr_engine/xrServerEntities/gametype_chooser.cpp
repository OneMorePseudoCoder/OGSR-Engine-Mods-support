#include "stdafx.h"

#include "gametype_chooser.h"
#include "xrServer_Objects_Abstract.h"
// old
enum ERPGameType
{ // [0..255]
    rpgtGameAny = u8(0),
    rpgtGameCount,
};

xr_token rpoint_game_type[] = {{"Any game", rpgtGameAny}, {0, 0}};