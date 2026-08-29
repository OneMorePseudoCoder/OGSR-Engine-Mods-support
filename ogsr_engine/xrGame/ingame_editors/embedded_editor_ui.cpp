#include "stdAfx.h"
#include "embedded_editor_ui.h"
#include "embedded_editor_helper.h"
#include "imgui_internal.h"
#include "../../xr_3da/device.h"
#include "ui_base.h"
#include "string_table.h"

CUIDebuggable::~CUIDebuggable() { UnregisterDebuggable(); }

void CUIDebuggable::RegisterDebuggable() { UI().Debugger().Register(this); }

void CUIDebuggable::UnregisterDebuggable() { UI().Debugger().Unregister(this); }

void CUIDebugger::Register(CUIDebuggable* debuggable)
{
    m_root_windows.emplace_back(debuggable);
}

void CUIDebugger::Unregister(CUIDebuggable* debuggable)
{
    const auto it = std::find(m_root_windows.begin(), m_root_windows.end(), debuggable);
    if (it != m_root_windows.end())
        m_root_windows.erase(it);
}

void CUIDebugger::SetSelected(CUIDebuggable* debuggable)
{
    m_state.selected = debuggable;
    m_state.newSelected = debuggable;
}

CUIDebugger::CUIDebugger()
{
    ImGui::SetAllocatorFunctions([](size_t size, void* /*user_data*/) { return xr_malloc(size); }, [](void* ptr, void* /*user_data*/) { xr_free(ptr); });
    // ImGui::SetCurrentContext(Device.editor().GetImGuiContext());
}

void CImGuiUIEditorWnd::Render()
{
    if (!RenderBegin())
    {
        RenderEnd();
        return;
    }

    static auto m_debugger = UI().Debugger();

    ImGui::Checkbox("Draw rects", &m_debugger.m_state.drawWndRects);
    ImGui::BeginDisabled(!m_debugger.m_state.drawWndRects);
    ImGui::Checkbox("Colored rects", &m_debugger.m_state.coloredRects);
    ImGui::EndDisabled();

    constexpr ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_BordersInner | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("Tree and properties", 2, flags))
    {
        ImGui::TableSetupColumn("Tree");
        ImGui::TableSetupColumn("Selected element properties", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();

        for (const auto& window : m_debugger.m_root_windows)
        {
            window->FillDebugTree(m_debugger.m_state);
            if (m_debugger.m_state.selected != m_debugger.m_state.newSelected)
                m_debugger.m_state.selected = m_debugger.m_state.newSelected;
        }

        ImGui::TableNextColumn();

        if (m_debugger.m_state.selected)
            m_debugger.m_state.selected->FillDebugInfo();

        ImGui::EndTable();
    }

    RenderEnd();
}