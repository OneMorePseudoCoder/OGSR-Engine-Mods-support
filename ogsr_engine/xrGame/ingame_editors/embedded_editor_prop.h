#pragma once
#include "embedded_editor_window.h"

class CObject;

class CImGuiPropEditorWnd : public CImGuiEditorWnd
{
public:
    CImGuiPropEditorWnd() : CImGuiEditorWnd("Properties Editor") {}
    CImGuiPropEditorWnd(LPSTR name) : CImGuiEditorWnd(name) {}
    void Render() override;
};