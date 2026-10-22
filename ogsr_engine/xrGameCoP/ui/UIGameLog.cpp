//=============================================================================
//  Filename:   UIGameLog.h
//	Created by Vitaly 'Mad Max' Maximov, mad-max@gsc-game.kiev.ua
//	Copyright 2005. GSC Game World
//	---------------------------------------------------------------------------
//  Multiplayer game log window
//=============================================================================
#include "stdafx.h"
#include "UIGameLog.h"
#include "UIXmlInit.h"
#include "UIPdaMsgListItem.h"
#include "UILines.h"

CUIGameLog::CUIGameLog()
{
    kill_msg_height = 20;
    txt_color = 0xff000000;
    m_pFont = nullptr;
}

CUITextWnd* CUIGameLog::AddLogMessage(LPCSTR msg)
{
    u32 curr_size = GetSize();

    CUITextWnd* pItem = nullptr;
    ADD_TEXT_TO_VIEW3(msg, pItem, this);
    pItem->SetFont(m_pFont);
    pItem->SetTextColor(txt_color);
    pItem->SetColorAnimation("ui_main_msgs_short", LA_ONLYALPHA | LA_TEXTCOLOR, 5000.0f);
    ForceUpdate();

    if (curr_size == 0)
    {
        const Fvector2 w_pos = {m_pad->GetWndPos().x, GetHeight()};
        m_pad->SetWndPos(w_pos);
    }

    return pItem;
}

CUIPdaMsgListItem* CUIGameLog::AddPdaMessage()
{
    u32 curr_size = GetSize();

    CUIPdaMsgListItem* pItem = xr_new<CUIPdaMsgListItem>();
    pItem->InitPdaMsgListItem(Fvector2().set(GetDesiredChildWidth(), 10.0f));
    pItem->SetColorAnimation("ui_main_msgs_short", LA_ONLYALPHA | LA_TEXTCOLOR | LA_TEXTURECOLOR);
    AddWindow(pItem, true);

    if (curr_size == 0)
    {
        const Fvector2 w_pos = {m_pad->GetWndPos().x, GetHeight()};
        m_pad->SetWndPos(w_pos);
    }

    return pItem;
}

void CUIGameLog::AddChatMessage(LPCSTR msg, LPCSTR author)
{
    string256 fullLine;
    xr_strconcat(fullLine, author, " ", msg);

    _TrimRight(fullLine);

    CUITextWnd* pItem = xr_new<CUITextWnd>();
    pItem->SetTextComplexMode(true);
    pItem->SetText(fullLine);
    pItem->SetCutWordsMode(true);
    pItem->SetFont(m_pFont);
    pItem->SetTextColor(txt_color);
    pItem->SetColorAnimation("ui_main_msgs_short", LA_ONLYALPHA | LA_TEXTCOLOR, 5000.0f);
    pItem->SetWidth(this->GetDesiredChildWidth());
    pItem->AdjustHeightToText();
    AddWindow(pItem, true);
}

void CUIGameLog::SetTextAtrib(CGameFont* pFont, u32 color)
{
    m_pFont = pFont;
    txt_color = color;
}

void CUIGameLog::Update()
{
    CUIScrollView::Update();
    toDelList.clear();

    auto it = m_pad->GetChildWndList().begin();
    auto it_e = m_pad->GetChildWndList().end();

    for (; it != it_e; ++it)
    {
        auto* pItem = smart_cast<CUILightAnimColorConroller*>(*it);

        if (!pItem->IsColorAnimationPresent())
            toDelList.push_back(*it);
    }

    if (!toDelList.empty())
    {
        // Delete elements
        {
            for (const auto& it : toDelList)
                RemoveWindow(it);
        }
        ForceScrollPosition();

        toDelList.clear();
    }
}