#include "stdafx.h"
#include "UIStatic.h"
#include "UIAnimatedStatic.h"

using namespace luabind;

void CUIStatic::script_register(lua_State* L)
{
    module(L)[class_<CUILines>("CUILines")
                  .def("SetFont", &CUILines::SetFont)
                  .def("SetText", &CUILines::SetText)
                  .def("SetTextST", &CUILines::SetTextST)
                  .def("GetText", &CUILines::GetText)
                  .def("SetElipsis", &CUILines::SetEllipsis)
                  .def("SetTextColor", &CUILines::SetTextColor),

              class_<CUIStatic, CUIWindow>("CUIStatic")
                  .def(constructor<>())

                  .def("TextControl", &CUIStatic::TextItemControl)

                  .def("SetText", &CUIStatic::SetText)
                  .def("SetTextST", &CUIStatic::SetTextST)

                  .def("GetText", &CUIStatic::GetText)

                  .def("SetTextOffset", &CUIStatic::SetTextOffset)
                  .def("SetTextX", [](CUIStatic* self, float x) { self->TextItemControl()->m_TextOffset.x = x; })
                  .def("SetTextY", [](CUIStatic* self, float y) { self->TextItemControl()->m_TextOffset.y = y; })
                  .def("GetTextX", [](CUIStatic* self) { return self->TextItemControl()->m_TextOffset.x; })
                  .def("GetTextY", [](CUIStatic* self) { return self->TextItemControl()->m_TextOffset.y; })

                  .def("SetColor", &CUIStatic::SetTextureColor)
                  .def("GetColor", &CUIStatic::GetTextureColor)

                  .def("SetFont", &CUIStatic::SetFont)
                  .def("GetFont", &CUIStatic::GetFont)

                  .def("SetTextColor", [](CUIStatic* self, int a, int r, int g, int b) { self->SetTextColor(color_argb(a, r, g, b)); })
                  .def("GetTextColor", &CUIStatic::GetTextColor)
                  .def("SetTextComplexMode", &CUIStatic::SetTextComplexMode)
                  .def("SetTextAlignment", &CUIStatic::SetTextAlignment)
                  .def("SetVTextAlignment", &CUIStatic::SetVTextAlignment)

                  .def("AdjustHeightToText", &CUIStatic::AdjustHeightToText)
                  .def("AdjustWidthToText", &CUIStatic::AdjustWidthToText)

                  .def("Init", [](CUIStatic* self, float x, float y, float width, float height) { self->SetWndRect({x, y, width, height}); })
                  .def("Init",
                       [](CUIStatic* self, const char* texture, float x, float y, float width, float height) {
                           self->SetWndRect({x, y, width, height});
                           self->InitTexture(texture);
                       })

                  .def("InitTexture", &CUIStatic::InitTexture)
                  .def("InitTexture", [](CUIStatic* self, pcstr texture) { self->InitTexture(texture); })
                  .def("InitTextureEx", &CUIStatic::InitTextureEx)
                  .def("InitTextureEx", [](CUIStatic* self, pcstr texture, pcstr shader) { self->InitTextureEx(texture, shader); })

                  .def("SetTextureColor", &CUIStatic::SetTextureColor)
                  .def("GetTextureColor", &CUIStatic::GetTextureColor)

                  .def("SetTextureOffset", &CUIStatic::SetTextureOffset)

                  .def("SetTextureRect", &CUIStatic::SetTextureRect_script)
                  .def("SetHint", &CUIStatic::SetHint)
                  .def("SetNoShaderCache", &CUIStatic::SetNoShaderCache)
                  .def("GetTextureRect", &CUIStatic::GetTextureRect_script)
                  .def("Heading", &CUIStatic::Heading)
                  .def("EnableHeading", &CUIStatic::EnableHeading)

                  .def("SetOriginalRect", &CUIStatic::SetTextureRect_script)
                  .def("GetOriginalRect", &CUIStatic::GetTextureRect_script)

                  .def("SetStretchTexture", &CUIStatic::SetStretchTexture)
                  .def("GetStretchTexture", &CUIStatic::GetStretchTexture)

                  .def("SetTextAlign",
                       [](CUIStatic* self, u32 align) {
                           self->TextItemControl()->SetTextAlignment(static_cast<ETextAlignment>(align));
                           self->TextItemControl()->GetFont()->SetAligment(static_cast<CGameFont::EAligment>(align));
                       })
                  .def("GetTextAlign", [](CUIStatic* self) -> u32 { return self->TextItemControl()->GetTextAlignment(); })

                  .def("SetHeading", &CUIStatic::SetHeading)
                  .def("GetHeading", &CUIStatic::GetHeading)

                  .def("ClipperOn", &CUIStatic::ClipperOn)
                  .def("ClipperOff", (void(CUIStatic::*)(void))&CUIStatic::ClipperOff)

                  .def("GetClipperState", &CUIStatic::GetClipperState)

                  .def("SetEllipsis", &CUIStatic::SetEllipsis)
                  .def("SetElipsis", &CUIStatic::SetEllipsis)
                  .def("SetElipsis", [](CUIStatic* self, int mode, int indent) { self->SetEllipsis(mode != 0); }),

              class_<CUITextWnd, CUIWindow>("CUITextWnd")
                  .def(constructor<>())
                  .def("AdjustHeightToText", &CUITextWnd::AdjustHeightToText)
                  .def("AdjustWidthToText", &CUITextWnd::AdjustWidthToText)
                  .def("SetText", &CUITextWnd::SetText)
                  .def("SetTextST", &CUITextWnd::SetTextST)
                  .def("GetText", &CUITextWnd::GetText)
                  .def("SetFont", &CUITextWnd::SetFont)
                  .def("GetFont", &CUITextWnd::GetFont)
                  .def("SetTextColor", &CUITextWnd::SetTextColor)
                  .def("GetTextColor", &CUITextWnd::GetTextColor)
                  .def("SetTextComplexMode", &CUITextWnd::SetTextComplexMode)
                  .def("GetTextComplexMode", &CUITextWnd::GetTextComplexMode)
                  .def("SetTextAlignment", &CUITextWnd::SetTextAlignment)
                  .def("SetVTextAlignment", &CUITextWnd::SetVTextAlignment)
                  .def("SetEllipsis", &CUITextWnd::SetEllipsis)
                  .def("SetTextOffset", &CUITextWnd::SetTextOffset),
              //		.def("",					&CUITextWnd::)

              class_<CUISleepStatic, CUIStatic>("CUISleepStatic").def(constructor<>())];
}