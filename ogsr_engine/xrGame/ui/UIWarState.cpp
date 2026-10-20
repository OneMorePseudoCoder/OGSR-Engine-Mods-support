////////////////////////////////////////////////////////////////////////////
//	Module 		: UIWarState.cpp
//	Created 	: 15.04.2008
//	Author		: Evgeniy Sokolov
//	Description : UI war state (PDA) window class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWarState.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "UIHelper.h"

UIWarState::UIWarState() {}

void UIWarState::InitXML(CUIXml& xml, LPCSTR att_name, CUIWindow* parent)
{
	VERIFY(parent);
	parent->AttachChild(this);
	SetAutoDelete(true);
	string256 buf;	

	CUIXmlInit::InitWindow(xml, att_name, 0, this);

	strconcat(sizeof(buf), buf, att_name, ":img");
	m_static = UIHelper::CreateStatic(xml, buf, this);

	set_hint_delay((u32)xml.ReadAttribInt(att_name, 0, "delay", 0));
}

void UIWarState::ClearInfo()
{
	SetVisible(false);
	set_hint_text_ST("");
}

bool UIWarState::UpdateInfo(LPCSTR icon, LPCSTR hint_text)
{
	if (!icon || !xr_strlen(icon))
	{
		return false;
	}

	SetVisible(true);
	m_static->InitTexture(icon);

	if (!hint_text || !xr_strlen(hint_text))
	{
		set_hint_text_ST("");
	}
	else
	{
		set_hint_text_ST(hint_text);
	}
	return true;
}

void UIWarState::Draw()
{
	if (IsShown())
	{
		inherited::Draw();
	}
}