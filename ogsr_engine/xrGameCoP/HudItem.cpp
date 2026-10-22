#include "StdAfx.h"
#include "HudItem.h"
#include "physic_item.h"
#include "Actor.h"
#include "ActorEffector.h"
#include "Missile.h"
#include "xrmessages.h"
#include "Level.h"
#include "Inventory.h"
#include "../xr_3da/CameraBase.h"
#include "player_hud.h"
#include "../xr_3da/SkeletonMotions.h"
#include "../xr_3da/GameMtlLib.h"
#include "HUDManager.h"
#include "ActorCondition.h"
#include "Weapon.h"
#include "../xrCore/_vector3d_ext.h"
#include "PDA.h"
#include <array>

extern float psHUD_FOV_def;

CHudItem::CHudItem()
{
    m_huditem_flags.zero();
    m_animation_slot = u32(-1);

    EnableHudInertion(TRUE);
    AllowHudInertion(TRUE);
    m_bobbing = std::make_unique<CWeaponBobbing>(this);

    m_bStopAtEndAnimIsRunning = false;
    m_current_motion_def = nullptr;
    m_started_rnd_anim_idx = u8(-1);
}

DLL_Pure* CHudItem::_construct()
{
    m_object = smart_cast<CPhysicItem*>(this);
    VERIFY(m_object);

    m_item = smart_cast<CInventoryItem*>(this);
    VERIFY(m_item);

    return m_object;
}

CHudItem::~CHudItem() {}

void CHudItem::Load(LPCSTR section)
{
    world_sect = section;

    hud_sect = pSettings->r_string(section, "hud");
    m_animation_slot = pSettings->r_u32(section, "animation_slot");

    m_bDisableBore = READ_IF_EXISTS(pSettings, r_bool, hud_sect, "disable_bore", false);

    if (!m_bDisableBore)
        m_sounds.LoadSound(section, "snd_bore", "sndBore", true);

    if (pSettings->line_exist(hud_sect, "allow_inertion"))
        EnableHudInertion(pSettings->r_bool(hud_sect, "allow_inertion"));

    if (pSettings->line_exist(hud_sect, "allow_bobbing"))
        allow_bobbing = pSettings->r_bool(hud_sect, "allow_bobbing");

    hud_recalc_koef = READ_IF_EXISTS(pSettings, r_float, hud_sect, "hud_recalc_koef",
                                     1.35f); // На калаше при 1.35 вроде норм смотрится, другим стволам возможно придется подбирать другие значения.

    m_nearwall_on = READ_IF_EXISTS(pSettings, r_bool, section, "nearwall_on", READ_IF_EXISTS(pSettings, r_bool, "features", "default_nearwall_on", true));

    if (m_nearwall_on)
    {
        constexpr bool IS_OGSR_GA{false};

        // Координаты офсетов для сдвига худа нашел в интернетах :)
        static std::array<std::tuple<float, float, Fvector, Fvector, float, float>, _CollisionWeaponTypesCount_> CollisionParamsBase{{
            // Min, Max dist, offset, rotate, HudFov, HudFov Aim
            {0.25f, 0.95f, {-0.0615f, -0.4380f, 0.1235f}, {-0.9219f, -0.0972f, 0.2525f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // Общие для всех оружий
            {0.25f, 0.70f, {-0.1000f, -0.5537f, 0.0350f}, {-1.0630f, 0.1751f, -0.0600f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.20f : 0.10f}, // Пистолеты
            {0.30f, 1.30f, {-0.0615f, -0.4380f, 0.1235f}, {-0.9219f, -0.0972f, 0.2525f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // СВД и прочие длинные снайперки
            {0.35f, 1.85f, {-0.0399f, 0.0929f, -0.0589f}, {0.3908f, 0.0488f, -0.0193f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // РПГ
            {0.25f, 0.80f, {0.0015f, -0.5655f, 0.1240f}, {-1.0319f, 0.0678f, 0.0700f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // РГ-6
            {0.25f, 0.80f, {-0.0406f, -0.4191f, 0.1718f}, {-0.8981f, -0.1101f, 0.4420f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // Гроза
            {0.25f, 0.80f, {-0.0335f, -0.4618f, 0.1098f}, {-0.9119f, -0.0973f, 0.4143f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // ФН2000
            {0.25f, 0.60f, {-0.0650f, -0.5170f, 0.0465f}, {-1.0405f, 0.1051f, -0.0350f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // БМ-16
            {0.30f, 0.50f, {-0.0025f, -0.4045f, -0.1415f}, {-0.7900f, 0.0100f, 0.f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // Болт
            {0.25f, 0.65f, {0.0120f, -0.4780f, -0.1150f}, {-0.6250f, -0.0725f, -0.1950f}, 0.5f, 1.f}, // Детектор
            {0.30f, 0.50f, {-0.0025f, -0.4045f, -0.1415f}, {-0.7900f, 0.0100f, 0.f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.25f : 0.15f}, // Нож, гранаты и прочее
            {0.25f, 0.70f, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, IS_OGSR_GA ? 0.5f : 0.25f, IS_OGSR_GA ? 0.40f : 0.30f}, // Бинокль
        }};
        const size_t type = GetWeaponTypeForCollision();
        const auto& CollisionParams = CollisionParamsBase.at(type);

        // Параметры изменения коллизии когда игрок стоит вплотную к стене
        m_nearwall_hud_offset_speed = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_hud_offset_speed", 0.1f); // Скорость поднятия\опускания ствола
        m_nearwall_dist_min =
            READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_min", std::get<0>(CollisionParams)); // Максимальное расстояние, на которое камера ГГ может упереться к стене
        m_nearwall_dist_max = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_max", std::get<1>(CollisionParams)); // Расстояние, ближе которого начинаем поднимать ствол
        m_nearwall_target_hud_offset = READ_IF_EXISTS(pSettings, r_fvector3, section, "nearwall_target_hud_offset", std::get<2>(CollisionParams)); // Максимальный оффсет худа
        m_nearwall_target_hud_rotate = READ_IF_EXISTS(pSettings, r_fvector3, section, "nearwall_target_hud_rotate", std::get<3>(CollisionParams)); // Максимальный поворот худа
        m_nearwall_target_hud_fov = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_target_hud_fov", std::get<4>(CollisionParams));
        m_nearwall_target_aim_hud_fov = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_target_aim_hud_fov", std::get<5>(CollisionParams));
        m_nearwall_speed_mod = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_speed_mod", 10.f);
    }

    // if (pSettings->line_exist(hud_sect, "hud_fov"))
    //     m_base_fov = READ_IF_EXISTS(pSettings, r_float, hud_sect, "hud_fov", 0.0f);
    // else
    m_base_fov = READ_IF_EXISTS(pSettings, r_float, section, "hud_fov", 0.0f);

    m_nearwall_last_hud_fov = m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def;

    ////////////////////////////////////////////
    m_strafe_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "strafe_hud_offset_pos", (Fvector{0.025f, 0.f, 0.f}));
    m_strafe_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "strafe_hud_offset_rot", (Fvector{0.f, 0.f, 5.5f}));

    m_strafe_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "strafe_aim_hud_offset_pos", (Fvector{0.f, 0.f, 0.f}));
    m_strafe_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "strafe_aim_hud_offset_rot", (Fvector{0.f, 0.f, 3.5f}));

    m_strafe_offset[2][0].set(READ_IF_EXISTS(pSettings, r_bool, section, "strafe_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "strafe_transition_time", 0.25f),
                              0.f);
    m_strafe_offset[2][1].set(READ_IF_EXISTS(pSettings, r_bool, section, "strafe_aim_enabled", true),
                              READ_IF_EXISTS(pSettings, r_float, section, "strafe_aim_transition_time", 0.15f), 0.f);

    ////////////////////////////////////////////
    m_lookout_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "lookout_hud_offset_pos", (Fvector{0.045f, 0.f, 0.f}));
    m_lookout_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "lookout_hud_offset_rot", (Fvector{0.f, 0.f, 10.f}));

    m_lookout_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "lookout_aim_hud_offset_pos", (Fvector{0.f, 0.f, 0.f}));
    m_lookout_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "lookout_aim_hud_offset_rot", (Fvector{0.f, 0.f, 15.f}));

    m_lookout_offset[2][0].set(READ_IF_EXISTS(pSettings, r_bool, section, "lookout_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "lookout_transition_time", 0.25f),
                               0.f);
    m_lookout_offset[2][1].set(READ_IF_EXISTS(pSettings, r_bool, section, "lookout_aim_enabled", true),
                               READ_IF_EXISTS(pSettings, r_float, section, "lookout_aim_transition_time", 0.15f), 0.f);
    ////////////////////////////////////////////
    m_jump_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "jump_hud_offset_pos", (Fvector{0.f, 0.05f, 0.03f}));
    m_jump_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "jump_hud_offset_rot", (Fvector{0.f, -10.f, -10.f}));

    m_jump_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "jump_aim_hud_offset_pos", (Fvector{0.f, 0.03f, 0.01f}));
    m_jump_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "jump_aim_hud_offset_rot", (Fvector{0.f, 2.5f, -3.f}));

    m_jump_offset[2][0].set(READ_IF_EXISTS(pSettings, r_bool, section, "jump_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "jump_transition_time", 0.35f), 0.f);
    m_jump_offset[2][1].set(READ_IF_EXISTS(pSettings, r_bool, section, "jump_aim_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "jump_aim_transition_time", 0.4f),
                            0.f);
    ////////////////////////////////////////////
    m_fall_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "fall_hud_offset_pos", (Fvector{0.f, -0.05f, 0.06f}));
    m_fall_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "fall_hud_offset_rot", (Fvector{0.f, 5.f, 0.f}));

    m_fall_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "fall_aim_hud_offset_pos", (Fvector{0.f, 0.03f, -0.01f}));
    m_fall_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "fall_aim_hud_offset_rot", (Fvector{0.f, -2.5f, 3.f}));
    ////////////////////////////////////////////
    m_landing_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "landing_hud_offset_pos", (Fvector{0.f, -0.2f, 0.03f}));
    m_landing_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "landing_hud_offset_rot", (Fvector{0.f, -5.f, 10.f}));

    m_landing_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "landing_aim_hud_offset_pos", (Fvector{0.f, -0.1f, 0.02f}));
    m_landing_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "landing_aim_hud_offset_rot", (Fvector{0.f, -2.5f, 5.f}));
    ////////////////////////////////////////////
    m_move_offset[0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "stay_hud_offset_pos", (Fvector{0.f, -0.03f, 0.f}));
    m_move_offset[1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "stay_hud_offset_rot", (Fvector{0.f, 0.5f, -3.f}));
    m_move_offset[2].set(READ_IF_EXISTS(pSettings, r_bool, section, "move_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "move_transition_time", 0.25f), 0.f);
    ////////////////////////////////////////////
    m_walk_offset[0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "walk_hud_offset_pos", (Fvector{-0.02f, -0.02f, -0.03f}));
    m_walk_offset[1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "walk_hud_offset_rot", (Fvector{0.f, 0.05f, -1.f}));
    m_walk_offset[2].set(READ_IF_EXISTS(pSettings, r_bool, section, "walk_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "walk_transition_time", 0.25f), 0.f);

    // Загрузка параметров инерции --#SM+# Begin--
    constexpr float PITCH_OFFSET_R = 0.0f; // Насколько сильно ствол смещается вбок (влево) при вертикальных поворотах камеры
    constexpr float PITCH_OFFSET_N = 0.0f; // Насколько сильно ствол поднимается\опускается при вертикальных поворотах камеры
    constexpr float PITCH_OFFSET_D = 0.02f; // Насколько сильно ствол приближается\отдаляется при вертикальных поворотах камеры
    constexpr float PITCH_LOW_LIMIT = -PI; // Минимальное значение pitch при использовании совместно с PITCH_OFFSET_N
    constexpr float ORIGIN_OFFSET = -0.03f; // Фактор влияния инерции на положение ствола (чем меньше, тем масштабней инерция)
    constexpr float ORIGIN_OFFSET_AIM = -0.02f; // (Для прицеливания)
    constexpr float TENDTO_SPEED = 5.f; // Скорость нормализации положения ствола
    constexpr float TENDTO_SPEED_AIM = 10.f; // (Для прицеливания)

    inertion_data.m_pitch_offset_r = READ_IF_EXISTS(pSettings, r_float, hud_sect, "pitch_offset_right", PITCH_OFFSET_R);
    inertion_data.m_pitch_offset_n = READ_IF_EXISTS(pSettings, r_float, hud_sect, "pitch_offset_up", PITCH_OFFSET_N);
    inertion_data.m_pitch_offset_d = READ_IF_EXISTS(pSettings, r_float, hud_sect, "pitch_offset_forward", PITCH_OFFSET_D);
    inertion_data.m_pitch_low_limit = READ_IF_EXISTS(pSettings, r_float, hud_sect, "pitch_offset_up_low_limit", PITCH_LOW_LIMIT);

    inertion_data.m_origin_offset = READ_IF_EXISTS(pSettings, r_float, hud_sect, "inertion_origin_offset", ORIGIN_OFFSET);
    inertion_data.m_origin_offset_aim = READ_IF_EXISTS(pSettings, r_float, hud_sect, "inertion_zoom_origin_offset", ORIGIN_OFFSET_AIM);
    inertion_data.m_tendto_speed = READ_IF_EXISTS(pSettings, r_float, hud_sect, "inertion_tendto_speed", TENDTO_SPEED);
    inertion_data.m_tendto_speed_aim = READ_IF_EXISTS(pSettings, r_float, hud_sect, "inertion_zoom_tendto_speed", TENDTO_SPEED_AIM);
    //--#SM+# End--
}

void CHudItem::PlaySound(LPCSTR alias, const Fvector& position, bool overlap)
{
    const Fvector& pos = GetHUDmode() ? Fvector().sub(position, ::Sound->listener_position()) : position;
    m_sounds.PlaySound(alias, pos, object().H_Root(), GetHUDmode(), false, overlap);
}

void CHudItem::renderable_Render(u32 context_id, IRenderable* root)
{
    UpdateXForm();

    const bool _hud_render = root && root->renderable_HUD() && GetHUDmode();
    if (_hud_render && !IsHidden())
        return;

    if (!object().H_Parent() || (!_hud_render && !IsHidden()))
    {
        on_renderable_Render(context_id, root);
    }
    else if (object().H_Parent())
    {
        const CInventoryOwner* owner = smart_cast<CInventoryOwner*>(object().H_Parent());
        VERIFY(owner);
        const CInventoryItem* self = smart_cast<CInventoryItem*>(this);
        if (owner->attached(self))
            on_renderable_Render(context_id, root);
    }
}

void CHudItem::SwitchState(u32 S)
{
    SetNextState(S);

    if (object().Local() && !object().getDestroy())
    {
        // !!! Just single entry for given state !!!
        NET_Packet P;
        object().u_EventGen(P, GE_WPN_STATE_CHANGE, object().ID());
        P.w_u8(u8(S));
        object().u_EventSend(P);
    }
}

void CHudItem::OnEvent(NET_Packet& P, u16 type)
{
    switch (type)
    {
    case GE_WPN_STATE_CHANGE: {
        u8 S;
        P.r_u8(S);
        OnStateSwitch(u32(S), GetState());
    }
    break;
    }
}

void CHudItem::OnStateSwitch(u32 S, u32 oldState)
{
    SetState(S);

    if (object().Remote())
        SetNextState(S);

    if (S == eBore)
    {
        SetPending(FALSE);

        PlayAnimBore();
        if (HudItemData())
        {
            const Fvector& P = HudItemData()->m_item_transform.c;
            m_sounds.PlaySound("sndBore", P, object().H_Root(), GetHUDmode(), false, false, m_started_rnd_anim_idx);
        }
    }
    else if (S == eHidden)
    {
        m_nearwall_last_hud_fov = m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def;
    }

    g_player_hud->updateMovementLayerState();
}

void CHudItem::OnAnimationEnd(u32 state)
{
    switch (state)
    {
    case eBore: {
        SwitchState(eIdle);
    }
    break;
    }
}

void CHudItem::PlayAnimBore() { PlayHUDMotion("anm_bore", TRUE, this, GetState()); }

bool CHudItem::ActivateItem()
{
    OnActiveItem();
    return true;
}

void CHudItem::DeactivateItem() { OnHiddenItem(); }
void CHudItem::OnMoveToRuck(const SInvItemPlace& prev) { SwitchState(eHidden); }

void CHudItem::SendDeactivateItem() { SendHiddenItem(); }
void CHudItem::SendHiddenItem()
{
    if (!object().getDestroy())
    {
        NET_Packet P;
        object().u_EventGen(P, GE_WPN_STATE_CHANGE, object().ID());
        P.w_u8(u8(eHiding));
        object().u_EventSend(P, net_flags(TRUE, TRUE, FALSE, TRUE));
    }
}

void CHudItem::UpdateCL()
{
    if (m_current_motion_def)
    {
        if (m_bStopAtEndAnimIsRunning)
        {
            const xr_vector<motion_marks>& marks = m_current_motion_def->marks;
            if (!marks.empty())
            {
                float motion_prev_time = ((float)m_dwMotionCurrTm - (float)m_dwMotionStartTm) / 1000.0f;
                float motion_curr_time = ((float)Device.dwTimeGlobal - (float)m_dwMotionStartTm) / 1000.0f;

                xr_vector<motion_marks>::const_iterator it = marks.begin();
                xr_vector<motion_marks>::const_iterator it_e = marks.end();
                for (; it != it_e; ++it)
                {
                    const motion_marks& M = (*it);
                    if (M.is_empty())
                        continue;

                    const motion_marks::interval* Iprev = M.pick_mark(motion_prev_time);
                    const motion_marks::interval* Icurr = M.pick_mark(motion_curr_time);
                    if (Iprev == NULL && Icurr != NULL /* || M.is_mark_between(motion_prev_time, motion_curr_time)*/)
                    {
                        OnMotionMark(m_startedMotionState, M);
                    }
                }
            }

            m_dwMotionCurrTm = Device.dwTimeGlobal;
            if (m_dwMotionCurrTm > m_dwMotionEndTm)
            {
                m_current_motion_def = NULL;
                m_dwMotionStartTm = 0;
                m_dwMotionEndTm = 0;
                m_dwMotionCurrTm = 0;
                m_bStopAtEndAnimIsRunning = false;
                OnAnimationEnd(m_startedMotionState);
            }
        }
    }

    AllowHudBobbing((psActorFlags.test(AF_WEAPON_BOBBING) && allow_bobbing) || (g_actor && g_actor->PsyAuraAffect));
}

void CHudItem::OnH_A_Chield() {}

void CHudItem::OnH_B_Chield()
{
    StopCurrentAnimWithoutCallback();

    m_nearwall_last_hud_fov = m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def;
}

void CHudItem::OnH_B_Independent(bool just_before_destroy)
{
    m_sounds.StopAllSounds();
    UpdateXForm();

    m_nearwall_last_hud_fov = m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def;

    // next code was commented
    /*
    if(HudItemData() && !just_before_destroy)
    {
        object().XFORM().set( HudItemData()->m_item_transform );
    }

    if (HudItemData())
    {
        g_player_hud->detach_item(this);
        Msg("---Detaching hud item [%s][%d]", this->HudSection().c_str(), this->object().ID());
    }*/
    // SetHudItemData			(NULL);
}

void CHudItem::OnH_A_Independent()
{
    if (HudItemData())
        g_player_hud->detach_item(this);
    StopCurrentAnimWithoutCallback();
}

void CHudItem::on_b_hud_detach() { m_sounds.StopAllSounds(); }

void CHudItem::on_a_hud_attach()
{
    if (m_current_motion_def)
    {
        PlayHUDMotion_noCB(m_current_motion, FALSE);
//#ifdef DEBUG
//		Msg("continue playing [%s][%d]",m_current_motion.c_str(), Device.dwFrame);
//#endif // #ifdef DEBUG
    }
//    else
//    {
//#ifdef DEBUG
//		Msg("no active motion");
//#endif // #ifdef DEBUG
//    }
}

u32 CHudItem::PlayHUDMotion(const shared_str& M, BOOL bMixIn, CHudItem* W, u32 state)
{
    u32 anim_time = PlayHUDMotion_noCB(M, bMixIn);
    if (anim_time > 0)
    {
        m_bStopAtEndAnimIsRunning = true;
        m_dwMotionStartTm = Device.dwTimeGlobal;
        m_dwMotionCurrTm = m_dwMotionStartTm;
        m_dwMotionEndTm = m_dwMotionStartTm + anim_time;
        m_startedMotionState = state;
    }
    else
        m_bStopAtEndAnimIsRunning = false;

    return anim_time;
}

u32 CHudItem::PlayHUDMotion_noCB(const shared_str& motion_name, BOOL bMixIn)
{
    m_current_motion = motion_name;

    if (bDebug && item().m_pInventory)
    {
        Msg("-[%s] as[%d] [%d]anim_play [%s][%d]", HudItemData() ? "HUD" : "Simulating", item().m_pInventory->GetActiveSlot(), item().object_id(), motion_name.c_str(),
            Device.dwFrame);
    }
    if (HudItemData())
    {
        return HudItemData()->anim_play(motion_name, bMixIn, m_current_motion_def, m_started_rnd_anim_idx);
    }
    else
    {
        m_started_rnd_anim_idx = 0;
        return g_player_hud->motion_length(motion_name, HudSection(), m_current_motion_def);
    }
}

void CHudItem::StopCurrentAnimWithoutCallback()
{
    m_dwMotionStartTm = 0;
    m_dwMotionEndTm = 0;
    m_dwMotionCurrTm = 0;
    m_bStopAtEndAnimIsRunning = false;
    m_current_motion_def = nullptr;
}

bool CHudItem::GetHUDmode() const
{
    if (object().H_Parent())
    {
        CActor* A = smart_cast<CActor*>(object().H_Parent());
        return A && A->HUDview() && HudItemData();
    }
    return false;
}

void CHudItem::PlayAnimIdle()
{
    if (TryPlayAnimIdle())
        return;

    PlayHUDMotion("anm_idle", TRUE, NULL, GetState());
}

bool CHudItem::TryPlayAnimIdle()
{
    // SIMP: Странная функция проверка, она ведь отключает не только мовинг но и спринт, что как бы не логично
    if (MovingAnimAllowedNow())
    {
        CActor* pActor = smart_cast<CActor*>(object().H_Parent());
        if (pActor)
        {
            CEntity::SEntityState st;
            pActor->g_State(st);
            if (st.bSprint)
            {
                PlayAnimIdleSprint();
                return true;
            }
            else if (AnmIdleMovingAllowed() && !st.bCrouch && pActor->AnyMove())
            {
                PlayAnimIdleMoving();
                return true;
            }
        }
    }
    return false;
}

void CHudItem::PlayAnimIdleMoving() { PlayHUDMotion("anm_idle_moving", TRUE, NULL, GetState()); }

void CHudItem::PlayAnimIdleSprint() { PlayHUDMotion("anm_idle_sprint", TRUE, NULL, GetState()); }

void CHudItem::OnMovementChanged(ACTOR_DEFS::EMoveCommand cmd)
{
    if (GetState() == eIdle && !m_bStopAtEndAnimIsRunning)
    {
        if ((cmd == ACTOR_DEFS::mcSprint) || (cmd == ACTOR_DEFS::mcAnyMove))
        {
            PlayAnimIdle();
            ResetSubStateTime();
        }
    }
}

bool CHudItem::AnmIdleMovingAllowed() const { return !HudBobbingAllowed() || Actor()->PsyAuraAffect; }

attachable_hud_item* CHudItem::HudItemData() const
{
    attachable_hud_item* hi = nullptr;
    if (!g_player_hud)
        return hi;

    hi = g_player_hud->attached_item(0);
    if (hi && hi->m_parent_hud_item == this)
        return hi;

    hi = g_player_hud->attached_item(1);
    if (hi && hi->m_parent_hud_item == this)
        return hi;

    return nullptr;
}

bool CHudItem::NeedBlendAnm()
{
    u32 state = GetState();
    return (state != eIdle && state != eHidden);
}

void CHudItem::PlayBlendAnm(LPCSTR name, float speed, float power, bool stop_old)
{
    u8 part = (object().cast_weapon()->IsZoomed() ? 2 : (g_player_hud->attached_item(1) ? 0 : 2));

    if (stop_old)
        g_player_hud->StopBlendAnm(name, true);
    g_player_hud->PlayBlendAnm(name, part, speed, power, false);
}

/*
bool CHudItem::used_cop_fire_point() const
{
    auto hd = HudItemData();
    return hd && hd->m_measures.useCopFirePoint;
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Сделано на базе увиденного в старом скрипте от Shoker.
// Перевёл его в движок и доработал под современные реалии для OGSR Engine: KRodin
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TODO: Желательно бы сделать визуальную настройку всех этих офсетов через hud_adjust
// TODO: Придумать более годный способ получения полиции и направления для коллизии болтов, детекторов, биноклей и ножей. У стволов есть shoot_point - там легко считать от него, в
// отличие от.
// TODO: Придумать какой-то способ получения нужной позиции для рейтрейса. Сейчас сделал не оптиматьно, по сути худ считается два раза за фрейм (не полностью, но такое решение всё
// равно выглядит плохо)
//              Эти костыли для двойного рассчета можно найти по skip_updated_frame, need_update_collision
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHudItem::CollisionAllowed() const 
{
    // Если выкл реалистичный прицел или у ствола ТЧ-стайл фейр поинт или ствол в режиме зума - коллизия работать не будет.
    return m_nearwall_on && psHUD_Flags.test(HUD_CROSSHAIR_HARD) /*&& used_cop_fire_point()*/;
}

void CHudItem::UpdateCollision(Fmatrix& trans)
{
    if (!CollisionAllowed())
        return;

    skip_updated_frame = Device.dwFrame;

    collide::rq_result RQ{nullptr, m_nearwall_dist_max * 1.1f, -1};
    const collide::ray_defs RD{GetPositionForCollision(), GetDirectionForCollision(), RQ.range, CDB::OPT_CULL, collide::rqtBoth};
    collide::rq_results RQR;

    Level().ObjectSpace.RayQuery(
        RQR, RD,
        [](collide::rq_result& result, LPVOID params) {
            // Копипаст из  CHUDTarget
            auto RQ = reinterpret_cast<collide::rq_result*>(params);

            if (result.O)
            {
                if (auto* e = smart_cast<CEntityAlive*>(result.O); e && e->g_Alive())
                    return TRUE;

                // if (smart_cast<CCustomShell*>(result.O))
                //     return TRUE;

                *RQ = result;
                return FALSE;
            }

            CDB::TRI* T = Level().ObjectSpace.GetStaticTris() + result.element;
            SGameMtl* mtl = GMLib.GetMaterialByIdx(T->material);
            if (mtl->Flags.is(SGameMtl::flPassable))
                return TRUE;

            *RQ = result;
            return FALSE;
        },
        &RQ, nullptr, Level().CurrentEntity());

    saved_rq_range = RQ.range;
    clamp(RQ.range, m_nearwall_dist_min, RQ.range);

    float dist_coef{};
    if (RQ.range <= m_nearwall_dist_max)
        dist_coef = 1.f - (RQ.range - m_nearwall_dist_min) / (m_nearwall_dist_max - m_nearwall_dist_min);

    const float fStepPerUpd = Device.fTimeDelta / m_nearwall_hud_offset_speed; // Величина изменение фактора смещения коллизии худа

    auto curr_offs = Fvector{}.mul(m_nearwall_target_hud_offset, dist_coef), curr_rot = Fvector{}.mul(m_nearwall_target_hud_rotate, dist_coef);

    // Плавный переход между смещением от коллизии и прицеливания
    curr_offs.mul(!IsZoomed());
    curr_rot.mul(!IsZoomed());

    // auto wpn = smart_cast<CWeapon*>(this);
    // curr_offs.mul(wpn && !wpn->LoweredActive);
    // curr_rot.mul(wpn && !wpn->LoweredActive);

    if (!curr_offs.similar(m_nearwall_last_pos, EPS))
        m_nearwall_last_pos.lerp(m_nearwall_last_pos, curr_offs, fStepPerUpd);

    if (!curr_rot.similar(m_nearwall_last_rot, EPS))
        m_nearwall_last_rot.lerp(m_nearwall_last_rot, curr_rot, fStepPerUpd);

    Fmatrix hud_rotation;
    hud_rotation.identity();
    hud_rotation.rotateX(m_nearwall_last_rot.x);

    Fmatrix hud_rotation_y;
    hud_rotation_y.identity();
    hud_rotation_y.rotateY(m_nearwall_last_rot.y);
    hud_rotation.mulA_43(hud_rotation_y);

    hud_rotation_y.identity();
    hud_rotation_y.rotateZ(m_nearwall_last_rot.z);
    hud_rotation.mulA_43(hud_rotation_y);

    hud_rotation.translate_over(m_nearwall_last_pos);
    trans.mulB_43(hud_rotation);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CHudItem::UpdateInertion(Fmatrix& trans)
{
    if (HudInertionEnabled() && HudInertionAllowed())
    {
        Fmatrix xform;
        Fvector& origin = trans.c;
        xform = trans;

        // calc difference
        Fvector diff_dir;
        diff_dir.sub(xform.k, inert_st_last_dir);

        // clamp by PI_DIV_2
        Fvector last;
        last.normalize_safe(inert_st_last_dir);
        const float dot = last.dotproduct(xform.k);
        if (dot < EPS)
        {
            Fvector v0;
            v0.crossproduct(inert_st_last_dir, xform.k);
            inert_st_last_dir.crossproduct(xform.k, v0);
            diff_dir.sub(xform.k, inert_st_last_dir);
        }

        // tend to forward
        float _tendto_speed, _origin_offset;
        if (GetCurrentHudOffsetIdx() > 0)
        { // Худ в режиме "Прицеливание"
            float factor = GetInertionFactor();
            _tendto_speed = inertion_data.m_tendto_speed_aim - (inertion_data.m_tendto_speed_aim - inertion_data.m_tendto_speed) * factor;
            _origin_offset = inertion_data.m_origin_offset_aim - (inertion_data.m_origin_offset_aim - inertion_data.m_origin_offset) * factor;
        }
        else
        { // Худ в режиме "От бедра"
            _tendto_speed = inertion_data.m_tendto_speed;
            _origin_offset = inertion_data.m_origin_offset;
        }

        // Фактор силы инерции
        const float power_factor = GetInertionPowerFactor();
        _tendto_speed *= power_factor;
        _origin_offset *= power_factor;

        inert_st_last_dir.mad(diff_dir, _tendto_speed * Device.fTimeDelta);
        origin.mad(diff_dir, -_origin_offset); // Инвертировал

        // pitch compensation
        float pitch = angle_normalize_signed(xform.k.getP());

        pitch *= GetInertionFactor();

        // Отдаление\приближение
        origin.mad(xform.k, -pitch * inertion_data.m_pitch_offset_d);

        // Сдвиг в противоположную часть экрана
        origin.mad(xform.i, -pitch * inertion_data.m_pitch_offset_r);

        // Подьём\опускание
        clamp(pitch, inertion_data.m_pitch_low_limit, PI);
        origin.mad(xform.j, -pitch * inertion_data.m_pitch_offset_n);
    }
}

// Обновление координат текущего худа
void CHudItem::UpdateHudAdditional(Fmatrix& trans, const bool need_update_collision)
{
    if (!need_update_collision)
        return;

    UpdateInertion(trans);

    Fvector summary_offset{}, summary_rotate{};

    attachable_hud_item* hi = HudItemData();
    u8 idx = GetCurrentHudOffsetIdx();
    const bool b_aiming = idx != hud_item_measures::m_hands_offset_type_normal;
    Fvector zr_offs = hi->m_measures.m_hands_offset[hud_item_measures::m_hands_offset_pos][idx];
    Fvector zr_rot = hi->m_measures.m_hands_offset[hud_item_measures::m_hands_offset_rot][idx];

    //============= Поворот ствола во время аима =============//
    {
        const float factor = Device.fTimeDelta / m_fZoomRotateTime;

        // I AM DEAD - Dirty hack from delayed exit from aim
        if (IsZoomed())
            m_fZoomRotationFactor += factor * 2.f;
        else
            m_fZoomRotationFactor -= factor * 2.f;

        clamp(m_fZoomRotationFactor, 0.f, 1.f);

        if (!zr_offs.similar(current_difference[0], EPS))
            current_difference[0].lerp(current_difference[0], zr_offs, factor * 2.f);

        if (!zr_rot.similar(current_difference[1], EPS))
            current_difference[1].lerp(current_difference[1], zr_rot, factor * 2.f);

        summary_offset.add(current_difference[0]);
    }
    //====================================================//

    auto pActor = smart_cast<const CActor*>(object().H_Parent());
    const u32 iMovingState = pActor->MovingState();
    idx = b_aiming ? 1ui8 : 0ui8;

    //============= Боковой стрейф с оружием =============//
    {
        const bool bEnabled = m_strafe_offset[2][idx].x;
        if (bEnabled)
        {
            float fStrafeMaxTime = m_strafe_offset[2][idx].y; // Макс. время в секундах, за которое произойдет смещение худа при стрейфах

            if (fStrafeMaxTime <= EPS)
                fStrafeMaxTime = 0.01f;

            const float fStepPerUpd = Device.fTimeDelta / fStrafeMaxTime; // Величина изменение фактора смещения худа при стрейфах

            Fvector current_moving_offs{}, current_moving_rot{};

            if (iMovingState & mcLStrafe) // Двигаемся влево
            {
                current_moving_offs.set(-m_strafe_offset[0][idx]);
                current_moving_rot.set(-m_strafe_offset[1][idx]);
            }
            else if (iMovingState & mcRStrafe) // Двигаемся вправо
            {
                current_moving_offs.set(m_strafe_offset[0][idx]);
                current_moving_rot.set(m_strafe_offset[1][idx]);
            }
            else // Двигаемся в любом другом направлении
            {
                current_moving_offs.set(Fvector{});
                current_moving_rot.set(Fvector{});
            }

            current_moving_rot.mul(-PI / 180.f); // Преобразуем углы в радианы

            if (!current_moving_offs.similar(current_strafe[0], EPS))
                current_strafe[0].lerp(current_strafe[0], current_moving_offs, fStepPerUpd);

            if (!current_moving_rot.similar(current_strafe[1], EPS))
                current_strafe[1].lerp(current_strafe[1], current_moving_rot, fStepPerUpd);

            summary_offset.add(current_strafe[0]);
            summary_rotate.add(current_strafe[1]);
        }
    }

    //=============== Эффекты прыжка ===============//
    {
        const bool bEnabled = m_jump_offset[2][idx].x;
        if (bEnabled)
        {
            float fJumpMaxTime = m_jump_offset[2][idx].y; // Макс. время в секундах, за которое произойдет смещение худа при прыжке

            if (fJumpMaxTime <= EPS)
                fJumpMaxTime = 0.01f;

            const float fStepPerUpd = Device.fTimeDelta / fJumpMaxTime; // Величина изменение фактора смещения худа при прыжке

            Fvector current_jump_offs{}, current_jump_rot{};

            if (iMovingState & mcJump) // Прыжок
            {
                current_jump_offs.set(m_jump_offset[0][idx]);
                current_jump_rot.set(m_jump_offset[1][idx]);
            }
            else if (iMovingState & mcFall) // Полет
            {
                current_jump_offs.set(m_fall_offset[0][idx]);
                current_jump_rot.set(m_fall_offset[1][idx]);
            }
            else if (iMovingState & mcLanding || iMovingState & mcLanding2) // Полет
            {
                current_jump_offs.set(m_landing_offset[0][idx]);
                current_jump_rot.set(m_landing_offset[1][idx]);
            }
            else
            {
                current_jump_offs.set(Fvector{});
                current_jump_rot.set(Fvector{});
            }

            float koef = iMovingState & mcLanding2 ? 1.3 : 1.0;
            current_jump_offs.mul(koef);
            current_jump_rot.mul(koef);
            current_jump_rot.mul(-PI / 180.f); // Преобразуем углы в радианы

            if (!current_jump_offs.similar(current_jump[0], EPS))
                current_jump[0].lerp(current_jump[0], current_jump_offs, fStepPerUpd);

            if (!current_jump_rot.similar(current_jump[1], EPS))
                current_jump[1].lerp(current_jump[1], current_jump_rot, fStepPerUpd);

            summary_offset.add(current_jump[0]);
            summary_rotate.add(current_jump[1]);
        }
    }

    //=============== Эффекты наклонов ===================//
    {
        const bool bEnabled = m_lookout_offset[2][idx].x;
        if (bEnabled)
        {
            float fLookoutMaxTime = m_lookout_offset[2][idx].y; // Макс. время в секундах, за которое мы наклонимся из центрального положения
            if (fLookoutMaxTime <= EPS)
                fLookoutMaxTime = 0.01f;

            const float fStepPerUpd = Device.fTimeDelta / fLookoutMaxTime; // Величина изменение фактора поворота

            float koef{1.f};
            if ((iMovingState & mcCrouch) && (iMovingState & mcAccel))
                koef = 0.5; // во сколько раз менять амплитуду при полном присяде
            else if (iMovingState & mcCrouch)
                koef = 0.75; // во сколько раз менять амплитуду при присяде

            Fvector current_lookout_offs{}, current_lookout_rot{};

            if ((iMovingState & mcLLookout) && !(iMovingState & mcRLookout)) // Выглядываем влево
            {
                current_lookout_offs.set(-m_lookout_offset[0][idx]);
                current_lookout_rot.set(-m_lookout_offset[1][idx]);
            }
            else if ((iMovingState & mcRLookout) && !(iMovingState & mcLLookout)) // Выглядываем вправо
            {
                current_lookout_offs.set(m_lookout_offset[0][idx]);
                current_lookout_rot.set(m_lookout_offset[1][idx]);
            }
            else
            {
                current_lookout_offs.set(Fvector{});
                current_lookout_rot.set(Fvector{});
            }

            current_lookout_offs.mul(koef);
            current_lookout_rot.mul(koef);
            current_lookout_rot.mul(-PI / 180.f); // Преобразуем углы в радианы

            if (!current_lookout_offs.similar(current_lookout[0], EPS))
                current_lookout[0].lerp(current_lookout[0], current_lookout_offs, fStepPerUpd);

            if (!current_lookout_rot.similar(current_lookout[1], EPS))
                current_lookout[1].lerp(current_lookout[1], current_lookout_rot, fStepPerUpd);

            summary_offset.add(current_lookout[0]);
            summary_rotate.add(current_lookout[1]);
        }
    }

    //=============== Эффекты стойки ===================//
    {
        const bool bEnabled = m_move_offset[2].x;
        if (bEnabled)
        {
            float fMoveMaxTime = m_move_offset[2].y; // Макс. время в секундах, за которое мы наклонимся из центрального положения
            if (fMoveMaxTime <= EPS)
                fMoveMaxTime = 0.01f;

            const float fStepPerUpd = Device.fTimeDelta / fMoveMaxTime; // Величина изменение фактора поворота

            float koef{};
            if ((iMovingState & mcCrouch) && (iMovingState & mcAccel))
                koef = 1.0; // во сколько раз менять амплитуду при полном присяде
            else if (iMovingState & mcCrouch)
                koef = 0.5; // во сколько раз менять амплитуду при присяде

            Fvector current_move_offs{}, current_move_rot{};

            if (iMovingState & mcCrouch) // Выглядываем влево
            {
                current_move_offs.set(m_move_offset[0]);
                current_move_rot.set(m_move_offset[1]);
            }
            else
            {
                current_move_offs.set(Fvector{});
                current_move_rot.set(Fvector{});
            }

            auto pda = smart_cast<CPda*>(this);
            auto missile = smart_cast<CMissile*>(this);

            current_move_offs.mul(koef);
            current_move_rot.mul(koef);
            current_move_offs.mul(!IsZoomed());
            current_move_rot.mul(!IsZoomed());
            current_move_offs.mul(!pda && !missile);
            current_move_rot.mul(!pda && !missile);
            current_move_rot.mul(-PI / 180.f); // Преобразуем углы в радианы

            if (!current_move_offs.similar(current_move[0], EPS))
                current_move[0].lerp(current_move[0], current_move_offs, fStepPerUpd);

            if (!current_move_rot.similar(current_move[1], EPS))
                current_move[1].lerp(current_move[1], current_move_rot, fStepPerUpd);

            summary_offset.add(current_move[0]);
            summary_rotate.add(current_move[1]);
        }
    }

    //=============== Эффекты ходьбы ===================//
    {
        const bool bEnabled = m_walk_offset[2].x;
        if (bEnabled)
        {
            float fWalkMaxTime = m_walk_offset[2].y; // Макс. время в секундах, за которое мы наклонимся из центрального положения
            if (fWalkMaxTime <= EPS)
                fWalkMaxTime = 0.01f;

            const float fStepPerUpd = Device.fTimeDelta / fWalkMaxTime; // Величина изменение фактора поворота

            float koef{1.f};
            if ((iMovingState & mcCrouch) && (iMovingState & mcAccel))
                koef = 0.5; // во сколько раз менять амплитуду при полном присяде
            else if (iMovingState & mcCrouch)
                koef = 0.7; // во сколько раз менять амплитуду при присяде

            Fvector current_walk_offs{}, current_walk_rot{};

            if (iMovingState & mcFwd)
            {
                current_walk_offs.set(m_walk_offset[0]);
                current_walk_rot.set(m_walk_offset[1]);
            }
            else if (iMovingState & mcBack)
            {
                current_walk_offs.set(m_walk_offset[0].x, m_walk_offset[0].y, -m_walk_offset[0].z);
                current_walk_rot.set(m_walk_offset[1]);
            }
            else
            {
                current_walk_offs.set(Fvector{});
                current_walk_rot.set(Fvector{});
            }

            auto pda = smart_cast<CPda*>(this);
            auto missile = smart_cast<CMissile*>(this);

            current_walk_offs.mul(koef);
            current_walk_rot.mul(koef);
            current_walk_offs.mul(!IsZoomed());
            current_walk_rot.mul(!IsZoomed());
            current_walk_offs.mul(!pda && !missile);
            current_walk_rot.mul(!pda && !missile);
            current_walk_rot.mul(-PI / 180.f); // Преобразуем углы в радианы

            if (!current_walk_offs.similar(current_walk[0], EPS))
                current_walk[0].lerp(current_walk[0], current_walk_offs, fStepPerUpd);

            if (!current_walk_rot.similar(current_walk[1], EPS))
                current_walk[1].lerp(current_walk[1], current_walk_rot, fStepPerUpd);

            summary_offset.add(current_walk[0]);
            summary_rotate.add(current_walk[1]);
        }
    }

    //================ Применение эффектов ===============//
    {
        // поворот с сохранением смещения by Zander
        Fvector _angle{}, _pos{trans.c};
        trans.getHPB(_angle);
        _angle.add(-summary_rotate);

        // Msg("##[%s] summary_rotate: [%f,%f,%f]", __FUNCTION__, summary_rotate.x, summary_rotate.y, summary_rotate.z);
        trans.setHPB(_angle.x, _angle.y, _angle.z);
        trans.c = _pos;

        Fmatrix hud_rotation;
        hud_rotation.identity();

        //================ Прицеливание ===============//
        hud_rotation.rotateX(current_difference[1].x);

        Fmatrix hud_rotation_part;
        hud_rotation_part.identity();
        hud_rotation_part.rotateY(current_difference[1].y);
        hud_rotation.mulA_43(hud_rotation_part);

        hud_rotation_part.identity();
        hud_rotation_part.rotateZ(current_difference[1].z);
        hud_rotation.mulA_43(hud_rotation_part);

        // Msg("--[%s] summary_offset: [%f,%f,%f]", __FUNCTION__, summary_offset.x, summary_offset.y, summary_offset.z);

        hud_rotation.translate_over(summary_offset);
        trans.mulB_43(hud_rotation);
    }
    //====================================================//

    UpdateCollision(trans);
}

float CHudItem::GetHudFov()
{
    // Рассчитываем HUD FOV от бедра (с учётом упирания в стены)
    if (m_nearwall_on)
    {
        // Получаем расстояние от камеры до точки в прицеле
        float dist = CollisionAllowed() ? saved_rq_range : HUD().GetCurrentRayQuery().range;

        // Интерполируем расстояние в диапазон от 0 (min) до 1 (max)
        clamp(dist, m_nearwall_dist_min, m_nearwall_dist_max);
        const float fDistanceMod = ((dist - m_nearwall_dist_min) / (m_nearwall_dist_max - m_nearwall_dist_min)); // 0.f ... 1.f

        // Плавно высчитываем итоговый FOV от бедра
        float src = m_nearwall_speed_mod * Device.fTimeDelta;
        clamp(src, 0.f, 1.f);

        const float fTrgFov = (IsZoomed() ? m_nearwall_target_aim_hud_fov : m_nearwall_target_hud_fov) +
            fDistanceMod * ((m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def) - (IsZoomed() ? m_nearwall_target_aim_hud_fov : m_nearwall_target_hud_fov));
        m_nearwall_last_hud_fov = m_nearwall_last_hud_fov * (1 - src) + fTrgFov * src;
    }

    return m_nearwall_last_hud_fov;
}

constexpr const char* BOBBING_SECT = "wpn_bobbing_effector";
constexpr float SPEED_REMINDER = 5.f;
CHudItem::CWeaponBobbing::CWeaponBobbing(CHudItem* parent) : parent_hud_item(parent)
{
    fTime = 0.f;
    fReminderFactor = 0.f;
    is_limping = false;

    m_fAmplitudeController = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "controller_amplitude", 0.003f);
    m_fAmplitudeRun = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "run_amplitude", 0.0075f);
    m_fAmplitudeWalk = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "walk_amplitude", 0.005f);
    m_fAmplitudeLimp = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "limp_amplitude", 0.011f);

    m_fSpeedRun = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "run_speed", 6.74f);
    m_fSpeedWalk = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "walk_speed", 6.26f);
    m_fSpeedLimp = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "limp_speed", 4.6f);

    m_fCrouchFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "crouch_k", 0.75f);
    m_fZoomFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "zoom_k", 1.f);
    m_fScopeZoomFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "scope_zoom_k", m_fZoomFactor);
}

void CHudItem::CWeaponBobbing::CheckState()
{
    dwMState = Actor()->MovingState();
    is_limping = Actor()->conditions().IsLimping();
    m_bZoomMode = Actor()->IsZoomAimingMode();
    fTime += Device.fTimeDelta;
}

void CHudItem::CWeaponBobbing::Update(Fmatrix& m, Fmatrix& m2)
{
    CheckState();

    // mmccxvii: Тряска рук при воздействии контролера
    const bool ControllerCondition = Actor()->PsyAuraAffect;

    if ((dwMState & ACTOR_DEFS::mcAnyMove) || ControllerCondition)

    {
        if (fReminderFactor < 1.f)
            fReminderFactor += SPEED_REMINDER * Device.fTimeDelta;
        else
            fReminderFactor = 1.f;
    }
    else
    {
        if (fReminderFactor > 0.f)
            fReminderFactor -= SPEED_REMINDER * Device.fTimeDelta;
        else
            fReminderFactor = 0.f;
    }

    if (!fsimilar(fReminderFactor, 0))
    {
        float k = (dwMState & ACTOR_DEFS::mcCrouch) ? m_fCrouchFactor : 1.f;
        float k2 = k;

        if (m_bZoomMode)
        {
            float zoom_factor = m_fZoomFactor;

            auto wpn = smart_cast<CWeapon*>(parent_hud_item);
            if (wpn && wpn->IsScopeAttached() && !wpn->IsGrenadeMode())
                zoom_factor = m_fScopeZoomFactor;

            k2 *= zoom_factor;
        }

        float A, ST;

        // mmccxvii: Тряска рук при воздействии контролера
        if (ControllerCondition)
        {
            A = m_fAmplitudeController * k;
            ST = m_fSpeedRun * 10 * fTime * k;
        }
        else if (isActorAccelerated(dwMState, m_bZoomMode))
        {
            A = m_fAmplitudeRun * k2;
            ST = m_fSpeedRun * fTime * k;
        }
        else if (is_limping)
        {
            A = m_fAmplitudeLimp * k2;
            ST = m_fSpeedLimp * fTime * k;
        }
        else
        {
            A = m_fAmplitudeWalk * k2;
            ST = m_fSpeedWalk * fTime * k;
        }

        float _sinA = _abs(_sin(ST) * A) * fReminderFactor;

        // mmccxvii: Тряска рук при воздействии контролера
        if (ControllerCondition)
            _sinA *= ::Random.randF(-1.0f, 1.0f);

        float _cosA = _cos(ST) * A * fReminderFactor;

        // применяем к матрице положения рук

        Fvector dangle;
        Fmatrix R, mR;

        dangle.x = _cosA;
        dangle.z = _cosA;
        dangle.y = _sinA;

        R.setHPB(dangle.x, dangle.y, dangle.z);

        m.c.y += _sinA;

        mR.mul(m, R);

        m.k.set(mR.k);
        m.j.set(mR.j);

        m2.c.y += _sinA;

        mR.mul(m2, R);

        m2.k.set(mR.k);
        m2.j.set(mR.j);
    }
}

void CHudItem::GetBoneOffsetPosDir(const shared_str& bone_name, Fvector& dest_pos, Fvector& dest_dir, const Fvector& offset)
{
    const u16 bone_id = HudItemData()->m_model->LL_BoneID(bone_name);
    ASSERT_FMT(bone_id != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), world_sect.c_str());
    Fmatrix& fire_mat = HudItemData()->m_model->LL_GetTransform(bone_id);
    fire_mat.transform_tiny(dest_pos, offset);
    HudItemData()->m_item_transform.transform_tiny(dest_pos);
    dest_pos.add(Device.vCameraPosition);
    dest_dir.set(0.f, 0.f, 1.f);
    HudItemData()->m_item_transform.transform_dir(dest_dir);
}

extern ENGINE_API float psHUD_FOV;

void CHudItem::CorrectDirFromWorldToHud(Fvector& worldPos)
{
    Fmatrix hud_project;
    hud_project.build_projection(deg2rad(psHUD_FOV <= 1.f ? psHUD_FOV * Device.fFOV : psHUD_FOV), Device.fASPECT, HUD_VIEWPORT_NEAR,
                                 g_pGamePersistent->Environment().CurrentEnv->far_plane);

    Device.mView.transform_dir(worldPos);
    hud_project.transform_dir(worldPos);

    Fmatrix{Device.mProject}.invert().transform_dir(worldPos);
    Fmatrix{Device.mView}.invert().transform_dir(worldPos);
}
