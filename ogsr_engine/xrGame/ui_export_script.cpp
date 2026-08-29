#include "stdafx.h"

#include "script_ui_registrator.h"
#include "MainMenu.h"

#include "UIGameCustom.h"
#include "ui/UIScriptWnd.h"
#include "ui/UIButton.h"
#include "ui/UIProgressBar.h"
#include "ui/UIEditBox.h"
#include "ui/UIMessageBox.h"
#include "ui/UIPropertiesBox.h"
#include "ui/UITabControl.h"
#include "ui/UIComboBox.h"
#include "ui/UIOptionsManagerScript.h"
#include "ScriptXMLInit.h"

using namespace luabind;

CMainMenu* MainMenu();


void UIRegistrator::script_register(lua_State* L)
{
    CUIWindow::script_register(L);
    CUIStatic::script_register(L);
    CUIButton::script_register(L);
    CUIProgressBar::script_register(L);
    CUIComboBox::script_register(L);
    CUIEditBox::script_register(L);
    CUITabControl::script_register(L);
    CUIMessageBox::script_register(L);
    CUIListBox::script_register(L);
    CUIDialogWndEx::script_register(L);
    CUIPropertiesBox::script_register(L);
    CUIOptionsManagerScript::script_register(L);
    CScriptXmlInit::script_register(L);
    CUIGameCustom::script_register(L);

    module(L)[

        class_<CGameFont>("CGameFont")
            .enum_("EAligment")[value("alLeft", int(CGameFont::alLeft)), value("alRight", int(CGameFont::alRight)), value("alCenter", int(CGameFont::alCenter))],

        class_<CMainMenu>("CMainMenu")
            .def("GetEngineVersion", &CMainMenu::GetEngineVersion)];

    module(L, "main_menu")[def("get_main_menu", &MainMenu)];
}
