#include "StdAfx.h"

#include "HudSound.h"

float psHUDSoundVolume = 1.0f;
float psHUDStepSoundVolume = 1.0f;
void InitHudSoundSettings()
{
    psHUDSoundVolume = pSettings->r_float("hud_sound", "hud_sound_vol_k");
    psHUDStepSoundVolume = pSettings->r_float("hud_sound", "hud_step_sound_vol_k");
}

void HUD_SOUND_ITEM::LoadSound(LPCSTR section, LPCSTR line, HUD_SOUND_ITEM& hud_snd, int type)
{
    hud_snd.m_activeSnd = NULL;
    hud_snd.sounds.clear();

    string256 sound_line;
    xr_strcpy(sound_line, line);
    int k = 0;
    while (pSettings->line_exist(section, sound_line))
    {
        hud_snd.sounds.push_back(SSnd());
        SSnd& s = hud_snd.sounds.back();

        LoadSound(section, sound_line, s.snd, type, &s.volume, &s.delay, &s.freq);
        xr_sprintf(sound_line, "%s%d", line, ++k);
    } // while
}

void HUD_SOUND_ITEM::LoadSound(LPCSTR section, LPCSTR line, ref_sound& snd, int type, float* volume, float* delay, float* freq)
{
    LPCSTR str = pSettings->r_string(section, line);
    string256 buf_str;

    int count = _GetItemCount(str);
    R_ASSERT(count);

    _GetItem(str, 0, buf_str);
    snd.create(buf_str, st_Effect, type);

    if (volume != NULL)
    {
        *volume = 1.f;
        if (count > 1)
        {
            _GetItem(str, 1, buf_str);
            if (xr_strlen(buf_str) > 0)
                *volume = (float)atof(buf_str);
        }
    }

    if (delay != NULL)
    {
        *delay = 0;
        if (count > 2)
        {
            _GetItem(str, 2, buf_str);
            if (xr_strlen(buf_str) > 0)
                *delay = (float)atof(buf_str);
        }
    }

    if (freq != NULL)
    {
        *freq = 1.f;
        if (count > 3)
        {
            _GetItem(str, 3, buf_str);
            if (xr_strlen(buf_str) > 0)
                *freq = (float)atof(buf_str);
        }
    }
}

void HUD_SOUND_ITEM::DestroySound(HUD_SOUND_ITEM& hud_snd)
{
    xr_vector<SSnd>::iterator it = hud_snd.sounds.begin();
    for (; it != hud_snd.sounds.end(); ++it)
        (*it).snd.destroy();
    hud_snd.sounds.clear();

    hud_snd.m_activeSnd = NULL;
}

void HUD_SOUND_ITEM::PlaySound(HUD_SOUND_ITEM& hud_snd, const Fvector& position, const CObject* parent, bool b_hud_mode, bool looped, bool overlap, u8 index)
{
    if (hud_snd.sounds.empty())
        return;

    if (!overlap)
        StopSound(hud_snd);

    u32 flags = b_hud_mode ? sm_2D : 0;
    if (looped)
        flags |= sm_Looped;

    // Alundaio: Sanity, don't allow PlaySound of index greater then the size, just play last index
    if (index == u8(-1))
        index = (u8)Random.randI(hud_snd.sounds.size());
    else if (index >= (u8)hud_snd.sounds.size())
        index = (u8)hud_snd.sounds.size() - 1;

    hud_snd.m_activeSnd = &hud_snd.sounds[index];

    float freq = hud_snd.m_activeSnd->freq;
    Fvector pos = (flags & sm_2D) ? Fvector{} : position;
    float vol = hud_snd.m_activeSnd->volume * (b_hud_mode ? psHUDSoundVolume : 1.0f);

    if (overlap)
    {
        hud_snd.m_activeSnd->snd.play_no_feedback(const_cast<CObject*>(parent), flags, hud_snd.m_activeSnd->delay, &pos, &vol, &freq);
    }
    else
    {
        hud_snd.m_activeSnd->snd.play_at_pos(const_cast<CObject*>(parent), pos, flags, hud_snd.m_activeSnd->delay);
        hud_snd.m_activeSnd->snd.set_volume(vol);
        hud_snd.m_activeSnd->snd.set_frequency(freq);
    }
}

void HUD_SOUND_ITEM::StopSound(HUD_SOUND_ITEM& hud_snd)
{
    for (auto& sound : hud_snd.sounds)
        sound.snd.stop();

    hud_snd.m_activeSnd = nullptr;
}

//----------------------------------------------------------
HUD_SOUND_COLLECTION::~HUD_SOUND_COLLECTION()
{
    for (auto& sound : m_sound_items)
    {
        HUD_SOUND_ITEM::StopSound(sound);
        HUD_SOUND_ITEM::DestroySound(sound);
    }

    m_sound_items.clear();
}

HUD_SOUND_ITEM* HUD_SOUND_COLLECTION::FindSoundItem(LPCSTR alias, bool b_assert)
{
    const auto it = std::find(m_sound_items.begin(), m_sound_items.end(), alias);

    if (it != m_sound_items.end())
        return &*it;

    R_ASSERT3(!b_assert, "sound item not found in collection", alias);
    return nullptr;
}

void HUD_SOUND_COLLECTION::PlaySound(LPCSTR alias, const Fvector& position, const CObject* parent, bool hud_mode, bool looped, bool overlap, u8 index)
{
    for (auto& sound : m_sound_items)
    {
        if (sound.m_b_exclusive)
            HUD_SOUND_ITEM::StopSound(sound);
    }

    HUD_SOUND_ITEM::PlaySound(*FindSoundItem(alias, true), position, parent, hud_mode, looped, overlap, index);
}

void HUD_SOUND_COLLECTION::StopSound(LPCSTR alias)
{
    HUD_SOUND_ITEM* snd_item = FindSoundItem(alias, true);
    HUD_SOUND_ITEM::StopSound(*snd_item);
}

void HUD_SOUND_COLLECTION::SetPosition(LPCSTR alias, const Fvector& pos)
{
    HUD_SOUND_ITEM* snd_item = FindSoundItem(alias, true);
    if (snd_item->playing())
        snd_item->set_position(pos);
}

void HUD_SOUND_COLLECTION::StopAllSounds()
{
    for (auto& sound : m_sound_items)
        HUD_SOUND_ITEM::StopSound(sound);
}

void HUD_SOUND_COLLECTION::LoadSound(LPCSTR section, LPCSTR line, LPCSTR alias, bool exclusive, int type)
{
    R_ASSERT(NULL == FindSoundItem(alias, false));
    m_sound_items.resize(m_sound_items.size() + 1);
    HUD_SOUND_ITEM& snd_item = m_sound_items.back();
    HUD_SOUND_ITEM::LoadSound(section, line, snd_item, type);
    snd_item.m_alias = alias;
    snd_item.m_b_exclusive = exclusive;
}

/*
It's usage is to play a group of sounds HUD_SOUND_ITEMs as if they were a single layered entity. This is a achieved by
wrapping the class around HUD_SOUND_COLLECTION and tagging them with the same alias. This way, when one for example
sndShot is played, it will play all the sound items with the same alias.
*/
//----------------------------------------------------------
HUD_SOUND_COLLECTION_LAYERED::~HUD_SOUND_COLLECTION_LAYERED()
{
    for (auto& it : m_sound_items)
        it.~HUD_SOUND_COLLECTION();

    m_sound_items.clear();
}

void HUD_SOUND_COLLECTION_LAYERED::StopAllSounds()
{
    for (auto& sound : m_sound_items)
        sound.StopAllSounds();
}

void HUD_SOUND_COLLECTION_LAYERED::StopSound(LPCSTR alias)
{
    for (auto& sound : m_sound_items)
    {
        if (sound.m_alias == alias)
            sound.StopSound(alias);
    }
}

void HUD_SOUND_COLLECTION_LAYERED::SetPosition(LPCSTR alias, const Fvector& pos)
{
    for (auto& sound : m_sound_items)
    {
        if (sound.m_alias == alias)
            sound.SetPosition(alias, pos);
    }
}

void HUD_SOUND_COLLECTION_LAYERED::PlaySound(LPCSTR alias, const Fvector& position, const CObject* parent, bool hud_mode, bool looped, u8 index)
{
    for (auto& sound : m_sound_items)
    {
        if (sound.m_alias == alias)
            sound.PlaySound(alias, position, parent, hud_mode, looped, index);
    }
}

HUD_SOUND_ITEM* HUD_SOUND_COLLECTION_LAYERED::FindSoundItem(LPCSTR alias, bool b_assert)
{
    for (auto& sound : m_sound_items)
    {
        if (sound.m_alias == alias)
            return sound.FindSoundItem(alias, b_assert);
    }

    return nullptr;
}

void HUD_SOUND_COLLECTION_LAYERED::LoadSound(LPCSTR section, LPCSTR line, LPCSTR alias, bool exclusive, int type) { LoadSound(pSettings, section, line, alias, exclusive, type); }

void HUD_SOUND_COLLECTION_LAYERED::LoadSound(CInifile const* ini, LPCSTR section, LPCSTR line, LPCSTR alias, bool exclusive, int type)
{
    LPCSTR str = ini->r_string(section, line);
    string256 buf_str;

    R_ASSERT(_GetItemCount(str));

    _GetItem(str, 0, buf_str);

    auto add_sound = [&](LPCSTR sec, LPCSTR ln) {
        auto& snd = m_sound_items.emplace_back();
        snd.LoadSound(sec, ln, alias, exclusive, type);
        snd.m_alias = alias;
    };

    if (ini->section_exist(buf_str))
    {
        string256 sound_line;
        int k = 1;

        xr_strcpy(sound_line, "snd_1_layer");

        while (ini->line_exist(buf_str, sound_line))
        {
            add_sound(buf_str, sound_line);
            xr_sprintf(sound_line, "snd_%d_layer", ++k);
        }
    }
    else // Compatibility with normal HUD_SOUND_COLLECTION sounds
    {
        add_sound(section, line);
    }
}