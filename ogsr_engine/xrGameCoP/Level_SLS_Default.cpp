#include "stdafx.h"
#include "Level.h"
#include "xrServer.h"

void CLevel::SLS_Default() // Default/Editor Load
{
    if (Server)
        Server->SLS_Default();
}
