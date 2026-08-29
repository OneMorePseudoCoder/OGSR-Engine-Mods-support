#ifndef GAMETYPE_CHOOSER_INCLUDED
#define GAMETYPE_CHOOSER_INCLUDED

#pragma once

// new
enum EGameIDs
{
    eGameIDNoGame = u32(0),
    eGameIDSingle = u32(1) << 0
};

class PropValue;
class PropItem;
using PropItemVec = xr_vector<PropItem*>;
using PropItemIt = PropItemVec::iterator;

struct GameTypeChooser
{
    Flags16 m_GameType;

    void SetDefaults() { m_GameType.one(); }
    bool MatchType(const u16 t) const { return (t == eGameIDNoGame) || !!m_GameType.test(t); };
};

#endif