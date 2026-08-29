#include "stdafx.h"
#include "Actor.h"
#include "Weapon.h"
#include "MercuryBall.h"
#include "Inventory.h"
#include "character_info.h"
#include "xr_level_controller.h"
#include "UsableScriptObject.h"
#include "CustomZone.h"
#include "../xr_3da/GameMtlLib.h"
#include "../xr_3da/CameraBase.h"
#include "ui/UIMainIngameWnd.h"
#include "UIGameCustom.h"
#include "Grenade.h"
#include "WeaponRPG7.h"
#include "ExplosiveRocket.h"
#include "game_cl_base.h"
#include "Level.h"
#include "clsid_game.h"
#include "HUDManager.h"
#include "PickupManager.h"

void CActor::feel_touch_new(CObject* O)
{
    CPhysicsShellHolder* sh = smart_cast<CPhysicsShellHolder*>(O);
    if (sh && sh->character_physics_support())
        m_feel_touch_characters++;
}

void CActor::feel_touch_delete(CObject* O)
{
    CPhysicsShellHolder* sh = smart_cast<CPhysicsShellHolder*>(O);
    if (sh && sh->character_physics_support())
        m_feel_touch_characters--;
}

BOOL CActor::feel_touch_contact(CObject* O)
{
    CInventoryItem* item = smart_cast<CInventoryItem*>(O);
    CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(O);

    if (item && item->Useful() && !item->object().H_Parent())
        return TRUE;

    if (inventory_owner && inventory_owner != smart_cast<CInventoryOwner*>(this))
    {
        // CPhysicsShellHolder* sh=smart_cast<CPhysicsShellHolder*>(O);
        // if(sh&&sh->character_physics_support()) m_feel_touch_characters++;
        return TRUE;
    }

    return (FALSE);
}

BOOL CActor::feel_touch_on_contact(CObject* O)
{
    CCustomZone* custom_zone = smart_cast<CCustomZone*>(O);
    if (!custom_zone)
        return (TRUE);

    Fsphere sphere;
    Center(sphere.P);
    sphere.R = 0.1f;
    if (custom_zone->inside(sphere))
        return (TRUE);

    return (FALSE);
}

BOOL g_b_COD_PickUpMode = TRUE;

void CActor::PickupModeUpdate()
{
    if (!pPickup->GetPickupMode())
        return; // kUSE key pressed

    // подбирание объекта
    if (g_b_COD_PickUpMode)
    {
        if (m_pObjectWeLookingAt && m_pObjectWeLookingAt->cast_inventory_item() && m_pObjectWeLookingAt->cast_inventory_item()->Useful() && m_pUsableObject &&
            !Level().m_feel_deny.is_object_denied(m_pObjectWeLookingAt))
        {
            m_pUsableObject->use(this);
            Game().SendPickUpEvent(ID(), m_pObjectWeLookingAt->ID());
        }
    }
    else
    {
        if (m_pObjectWeLookingAt && m_pObjectWeLookingAt->cast_inventory_item() && m_pObjectWeLookingAt->cast_inventory_item()->Useful() &&
            m_pObjectWeLookingAt->cast_inventory_item()->CanTake() && !Level().m_feel_deny.is_object_denied(m_pObjectWeLookingAt))
        {
            if (m_pUsableObject && !m_pUsableObject->nonscript_usable())
                m_pUsableObject->use(this);
            Game().SendPickUpEvent(ID(), m_pObjectWeLookingAt->ID());
        }
    }

    pPickup->RenderInfo();
}

void CActor::PickupModeUpdate_COD()
{
    if (Level().CurrentViewEntity() != this || !g_b_COD_PickUpMode)
        return;

    if (!g_Alive() || eacFirstEye != cam_active)
    {
        CurrentGameUI()->UIMainIngameWnd->SetPickUpItem(nullptr);
        return;
    };

    CFrustum frustum;
    frustum.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB | FRUSTUM_P_FAR);

    ISpatialResult.clear();
    g_SpatialSpace->q_frustum(ISpatialResult, 0, STYPE_COLLIDEABLE, frustum);

    float maxlen = 1000.0f;
    CInventoryItem* pNearestItem = nullptr;

    for (u32 o_it = 0; o_it < ISpatialResult.size(); o_it++)
    {
        ISpatial* spatial = ISpatialResult[o_it];
        CInventoryItem* pIItem = smart_cast<CInventoryItem*>(spatial->dcast_CObject());

        if (0 == pIItem)
            continue;
        if (pIItem->object().H_Parent() != nullptr)
            continue;
        if (!pIItem->CanTake())
            continue;
        if (smart_cast<CExplosiveRocket*>(&pIItem->object()))
            continue;

        CMissile* pMissile = smart_cast<CMissile*>(spatial->dcast_CObject());
        if (pMissile && !pMissile->Useful())
            continue;

        Fvector A, B, tmp;
        pIItem->object().Center(A);
        if (A.distance_to_sqr(Position()) > 4)
            continue;

        tmp.sub(A, cam_Active()->vPosition);
        B.mad(cam_Active()->vPosition, cam_Active()->vDirection, tmp.dotproduct(cam_Active()->vDirection));
        float len = B.distance_to_sqr(A);
        if (len > 1)
            continue;

        if (maxlen > len && !pIItem->object().getDestroy())
        {
            maxlen = len;
            pNearestItem = pIItem;
        };
    }

    if (pNearestItem)
    {
        CFrustum frustum;
        frustum.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB | FRUSTUM_P_FAR);
        if (!pPickup->CanPickItem(frustum, Device.vCameraPosition, &pNearestItem->object()))
            pNearestItem = nullptr;
    }
    if (pNearestItem && pNearestItem->cast_game_object())
    {
        if (Level().m_feel_deny.is_object_denied(pNearestItem->cast_game_object()))
            pNearestItem = nullptr;
    }
    if (pNearestItem && pNearestItem->cast_game_object())
    {
        if (!pNearestItem->cast_game_object()->getVisible())
            pNearestItem = nullptr;
    }

    CurrentGameUI()->UIMainIngameWnd->SetPickUpItem(pNearestItem);

    if (pNearestItem && pPickup->GetPickupMode())
    {
        CUsableScriptObject* pUsableObject = smart_cast<CUsableScriptObject*>(pNearestItem);
        if (pUsableObject && (!m_pUsableObject))
            pUsableObject->use(this);

        // подбирание объекта
        Game().SendPickUpEvent(ID(), pNearestItem->object().ID());
    }
};

void CActor::feel_sound_new(CObject* who, int type, CSound_UserDataPtr user_data, const Fvector& Position, float power)
{
    if (who == this)
        m_snd_noise = _max(m_snd_noise, power);
}

void CActor::Feel_Grenade_Update(float rad)
{
    // Find all nearest objects
    Fvector pos_actor;
    Center(pos_actor);

    q_nearest.clear();
    g_pGameLevel->ObjectSpace.GetNearest(q_nearest, pos_actor, rad, nullptr);

    // select only grenade
    for (auto it : q_nearest)
    {
        if (it->getDestroy())
            continue; // Don't touch candidates for destroy

        CGrenade* grn = smart_cast<CGrenade*>(it);
        if (!grn || grn->Initiator() == ID() || grn->Useful())
        {
            continue;
        }
        if (grn->time_from_begin_throw() < m_fFeelGrenadeTime)
        {
            continue;
        }
        if (HUD().AddGrenade_ForMark(grn))
        {
            //.	Msg("__ __ Add new grenade! id = %d ", grn->ID() );
        }
    }

    HUD().Update_GrenadeView(pos_actor);
}
