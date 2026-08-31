#include "stdafx.h"
#include "UIGameCustom.h"
#include "Level.h"
#include "ui/UIXmlInit.h"
#include "ui/UIStatic.h"
#include "object_broker.h"
#include "string_table.h"

#include "InventoryOwner.h"
#include "ui/UIActorMenu.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIMessagesWindow.h"
#include "ui/UIHudStatesWnd.h"
#include "Actor.h"
#include "Inventory.h"
#include "game_cl_base.h"

#include "../xr_3da/x_ray.h"

#include "ui\UICellItem.h" //Alundaio

#include <imgui.h>

EGameIDs ParseStringToGameType(LPCSTR str);

struct predicate_find_stat
{
    LPCSTR m_id;
    predicate_find_stat(LPCSTR id) : m_id(id) {}
    bool operator()(StaticDrawableWrapper* s) { return (s->m_name == m_id); }
};

CUIGameCustom::CUIGameCustom() : m_msgs_xml(nullptr), m_ActorMenu(nullptr), m_PdaMenu(nullptr), m_window(nullptr), UIMainIngameWnd(nullptr), m_pMessagesWnd(nullptr)
{
    ShowGameIndicators(true);
    ShowCrosshair(true);
}

bool g_b_ClearGameCaptions = false;

CUIGameCustom::~CUIGameCustom()
{
    delete_data(m_custom_statics);
    g_b_ClearGameCaptions = false;
}

void CUIGameCustom::OnFrame()
{
    CDialogHolder::OnFrame();

    for (auto item : m_custom_statics)
        item->Update();

    auto comparer = [](const StaticDrawableWrapper* s1, const StaticDrawableWrapper* s2) { return s1->IsActual() > s2->IsActual(); };

    std::sort(m_custom_statics.begin(), m_custom_statics.end(), comparer);

    while (!m_custom_statics.empty() && !m_custom_statics.back()->IsActual())
    {
        delete_data(m_custom_statics.back());
        m_custom_statics.pop_back();
    }

    if (g_b_ClearGameCaptions)
    {
        delete_data(m_custom_statics);
        g_b_ClearGameCaptions = false;
    }
    m_window->Update();

    // update windows
    UIMainIngameWnd->Update();
    m_pMessagesWnd->Update();
}

void CUIGameCustom::Render()
{
    st_vec_it it = m_custom_statics.begin();
    st_vec_it it_e = m_custom_statics.end();
    for (; it != it_e; ++it)
        (*it)->Draw();

    m_window->Draw();

    CEntity* pEntity = smart_cast<CEntity*>(Level().CurrentEntity());
    if (pEntity)
    {
        CActor* pActor = smart_cast<CActor*>(pEntity);
        if (pActor && pActor->HUDview() && pActor->g_Alive())
        {
            u16 ISlot = pActor->inventory().FirstSlot();
            u16 ESlot = pActor->inventory().LastSlot();

            for (; ISlot <= ESlot; ++ISlot)
            {
                PIItem itm = pActor->inventory().ItemFromSlot(ISlot);
                if (itm && itm->render_item_ui_query())
                    itm->render_item_ui();
            }
        }

        if (GameIndicatorsShown() && psHUD_Flags.is(HUD_DRAW | HUD_DRAW_RT))
            UIMainIngameWnd->Draw();
    }

    m_pMessagesWnd->Draw();

    DoRenderDialogs();
}

StaticDrawableWrapper* CUIGameCustom::AddCustomStatic(LPCSTR id, bool bSingleInstance)
{
    if (bSingleInstance)
    {
        st_vec::iterator it = std::find_if(m_custom_statics.begin(), m_custom_statics.end(), predicate_find_stat(id));
        if (it != m_custom_statics.end())
            return (*it);
    }

    CUIXmlInit xml_init;
    m_custom_statics.push_back(xr_new<StaticDrawableWrapper>());
    StaticDrawableWrapper* sss = m_custom_statics.back();

    sss->m_static = xr_new<CUIStatic>();
    sss->m_name = id;
    xml_init.InitStatic(*m_msgs_xml, id, 0, sss->m_static);
    float ttl = m_msgs_xml->ReadAttribFlt(id, 0, "ttl", -1);
    if (ttl > 0.0f)
        sss->m_endTime = Device.fTimeGlobal + ttl;

    return sss;
}

StaticDrawableWrapper* CUIGameCustom::GetCustomStatic(LPCSTR id)
{
    st_vec::iterator it = std::find_if(m_custom_statics.begin(), m_custom_statics.end(), predicate_find_stat(id));
    if (it != m_custom_statics.end())
        return (*it);

    return nullptr;
}

void CUIGameCustom::RemoveCustomStatic(LPCSTR id)
{
    st_vec::iterator it = std::find_if(m_custom_statics.begin(), m_custom_statics.end(), predicate_find_stat(id));
    if (it != m_custom_statics.end())
    {
        delete_data(*it);
        m_custom_statics.erase(it);
    }
}

void CUIGameCustom::OnInventoryAction(PIItem item, u16 action_type)
{
    if (m_ActorMenu->IsShown())
        m_ActorMenu->OnInventoryAction(item, action_type);
}

#include "ui/UIGameTutorial.h"

extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;

bool CUIGameCustom::ShowActorMenu()
{
    if (m_ActorMenu->IsShown())
    {
        m_ActorMenu->HideDialog();
    }
    else
    {
        HidePdaMenu();
        CInventoryOwner* pIOActor = smart_cast<CInventoryOwner*>(Level().CurrentViewEntity());
        VERIFY(pIOActor);
        m_ActorMenu->SetActor(pIOActor);
        m_ActorMenu->SetMenuMode(mmInventory);
        m_ActorMenu->ShowDialog(true);
    }
    return true;
}

void CUIGameCustom::HideActorMenu()
{
    if (m_ActorMenu->IsShown())
    {
        m_ActorMenu->HideDialog();
    }
}

void CUIGameCustom::HideMessagesWindow()
{
    if (m_pMessagesWnd->IsShown())
        m_pMessagesWnd->Show(false);
}

void CUIGameCustom::ShowMessagesWindow()
{
    if (!m_pMessagesWnd->IsShown())
        m_pMessagesWnd->Show(true);
}

bool CUIGameCustom::ShowPdaMenu()
{
    HideActorMenu();
    m_PdaMenu->ShowDialog(true);
    return true;
}

void CUIGameCustom::HidePdaMenu()
{
    if (m_PdaMenu->IsShown())
    {
        m_PdaMenu->HideDialog();
    }
}

void CUIGameCustom::SetClGame(game_cl_GameState* g) { g->SetGameUI(this); }

void CUIGameCustom::UnLoad()
{
    xr_delete(m_msgs_xml);
    xr_delete(m_ActorMenu);
    xr_delete(m_PdaMenu);
    xr_delete(m_window);
    xr_delete(UIMainIngameWnd);
    xr_delete(m_pMessagesWnd);
}

void CUIGameCustom::Load()
{
    if (g_pGameLevel)
    {
        R_ASSERT(nullptr == m_msgs_xml);
        m_msgs_xml = xr_new<CUIXml>();
        m_msgs_xml->Load(CONFIG_PATH, UI_PATH, "ui_custom_msgs.xml");

        R_ASSERT(nullptr == m_ActorMenu);
        m_ActorMenu = xr_new<CUIActorMenu>();

        R_ASSERT(nullptr == m_PdaMenu);
        m_PdaMenu = xr_new<CUIPdaWnd>();

        R_ASSERT(nullptr == m_window);
        m_window = xr_new<CUIWindow>();

        R_ASSERT(nullptr == UIMainIngameWnd);
        UIMainIngameWnd = xr_new<CUIMainIngameWnd>();
        UIMainIngameWnd->Init();

        R_ASSERT(nullptr == m_pMessagesWnd);
        m_pMessagesWnd = xr_new<CUIMessagesWindow>();

        Init(0);
        Init(1);
        Init(2);
    }
}

void CUIGameCustom::OnConnected()
{
    if (g_pGameLevel)
    {
        if (!UIMainIngameWnd)
            Load();

        UIMainIngameWnd->OnConnected();
    }
}

void CUIGameCustom::CommonMessageOut(LPCSTR text) { m_pMessagesWnd->AddLogMessage(text); }
void CUIGameCustom::UpdatePda() { PdaMenu().UpdatePda(); }

void CUIGameCustom::update_fake_indicators(u8 type, float power) { UIMainIngameWnd->get_hud_states()->FakeUpdateIndicatorType(type, power); }

void CUIGameCustom::enable_fake_indicators(bool enable) { UIMainIngameWnd->get_hud_states()->EnableFakeIndicators(enable); }

StaticDrawableWrapper::StaticDrawableWrapper()
{
    m_static = nullptr;
    m_endTime = -1.0f;
}

void StaticDrawableWrapper::destroy() { delete_data(m_static); }

bool StaticDrawableWrapper::IsActual() const
{
    if (m_endTime < 0)
        return true;
    return (Device.fTimeGlobal < m_endTime);
}

void StaticDrawableWrapper::SetText(LPCSTR text)
{
    m_static->Show(text != nullptr);
    if (text)
    {
        m_static->TextItemControl()->SetTextST(text);
        m_static->ResetColorAnimation();
    }
}

void StaticDrawableWrapper::Draw()
{
    if (m_static->IsShown())
        m_static->Draw();
}

void StaticDrawableWrapper::Update()
{
    if (IsActual() && m_static->IsShown())
        m_static->Update();
}

bool CUIGameCustom::FillDebugTree(const CUIDebugState& debugState)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

    if (debugState.selected == this)
        flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::PushID(this);

    const bool open = ImGui::TreeNodeEx("CUIGameCustom", flags, "Game UI (%s)", CUIGameCustom::GetDebugType());

    if (ImGui::IsItemClicked())
        debugState.select(this);

    if (open)
    {
        CDialogHolder::FillDebugTree(debugState);

        m_window->FillDebugTree(debugState);
        ActorMenu().FillDebugTree(debugState);
        PdaMenu().FillDebugTree(debugState);
        UIMainIngameWnd->FillDebugTree(debugState);
        m_pMessagesWnd->FillDebugTree(debugState);

        for (const auto& custom_static : m_custom_statics)
        {
            if (custom_static)
                custom_static->wnd()->FillDebugTree(debugState);
        }
        ImGui::TreePop();
    }

    ImGui::PopID();

    return open;
}

void CUIGameCustom::FillDebugInfo()
{
    CDialogHolder::FillDebugInfo();
    if (ImGui::CollapsingHeader(CUIGameCustom::GetDebugType()))
    {
        ImGui::Checkbox("Show game indicators", &m_bShowGameIndicators);
    }
}

//-Alundaio
void CUIGameCustom::UpdateActorMenu()
{
	if (ActorMenu().IsShown())
	{
		ActorMenu().UpdateActor();
		ActorMenu().RefreshCurrentItemCell();
	}
}

CScriptGameObject* CUIGameCustom::CurrentItemAtCell()
{
    CUICellItem* itm = ActorMenu().CurrentItem();
	if (!itm->m_pData)
		return (0);

	PIItem IItm = (PIItem)itm->m_pData;
	if (!IItm)
		return (0);

	CGameObject* GO = smart_cast<CGameObject*>(IItm);

	if (GO)
		return GO->lua_game_object();

	return (0);
}
//-Alundaio