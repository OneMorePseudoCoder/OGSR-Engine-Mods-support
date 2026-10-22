#include "stdafx.h"
#include "BoneProtections.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xr_3da/bone.h"
#include "Level.h"

float SBoneProtections::getBoneProtection(s16 bone_id)
{
    storage_it it = m_bones_koeff.find(bone_id);
    if (it != m_bones_koeff.end())
        return it->second.koeff;
    else
        return m_default.koeff;
}

float SBoneProtections::getBoneArmor(s16 bone_id)
{
    storage_it it = m_bones_koeff.find(bone_id);
    if (it != m_bones_koeff.end())
        return it->second.armor;
    else
        return m_default.armor;
}

BOOL SBoneProtections::getBonePassBullet(s16 bone_id)
{
    storage_it it = m_bones_koeff.find(bone_id);
    if (it != m_bones_koeff.end())
        return it->second.BonePassBullet;
    else
        return m_default.BonePassBullet;
}

void SBoneProtections::reload(const shared_str& bone_sect, IKinematics* kinematics)
{
    VERIFY(kinematics);
    m_bones_koeff.clear();

    m_fHitFracNpc = READ_IF_EXISTS(pSettings, r_float, bone_sect, "hit_fraction_npc", 0.1f);

    m_default.koeff = 1.0f;
    m_default.armor = 0.0f;
    m_default.BonePassBullet = FALSE;

    const auto& protections = pSettings->r_section(bone_sect);
    for (const auto& [key, val] : protections.Ordered_Data)
    {
        string256 buffer;

        BoneProtection BP;

        BP.koeff = (float)atof(_GetItem(val.c_str(), 0, buffer));
        BP.armor = (float)atof(_GetItem(val.c_str(), 1, buffer));
        BP.BonePassBullet = (BOOL)(atoi(_GetItem(val.c_str(), 2, buffer)) > 0.5f);

        if (!xr_strcmp(key.c_str(), "default"))
        {
            m_default = BP;
        }
        else
        {
            if (!xr_strcmp(key.c_str(), "hit_fraction"))
                continue;

            s16 bone_id = kinematics->LL_BoneID(key);
            R_ASSERT2(BI_NONE != bone_id, key.c_str());
            m_bones_koeff.insert(std::make_pair(bone_id, BP));
        }
    }
}

void SBoneProtections::add(const shared_str& bone_sect, IKinematics* kinematics)
{
    VERIFY(kinematics);
    m_fHitFracNpc += READ_IF_EXISTS(pSettings, r_float, bone_sect.c_str(), "hit_fraction_npc", 0.0f);

    const auto& protections = pSettings->r_section(bone_sect);
    for (const auto& [key, val] : protections.Ordered_Data)
    {
        if (!xr_strcmp(key.c_str(), "hit_fraction"))
            continue;

        string256 buffer;
        if (!xr_strcmp(key.c_str(), "default"))
        {
            BoneProtection& BP = m_default;
            BP.koeff += (float)atof(_GetItem(val.c_str(), 0, buffer));
            BP.armor += (float)atof(_GetItem(val.c_str(), 1, buffer));
        }
        else
        {
            s16 bone_id = kinematics->LL_BoneID(key);
            R_ASSERT2(BI_NONE != bone_id, key.c_str());
            BoneProtection& BP = m_bones_koeff[bone_id];
            BP.koeff += (float)atof(_GetItem(val.c_str(), 0, buffer));
            BP.armor += (float)atof(_GetItem(val.c_str(), 1, buffer));
        }
    }
}
