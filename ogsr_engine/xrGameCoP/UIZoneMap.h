#pragma once

#include "ui/UIStatic.h"

class CActor;
class CUIMiniMap;

class CUIZoneMap
{
public:
    bool visible{ true };

    CUIMiniMap* m_activeMap{};
    float m_fScale;

    CUIStatic* m_background{};
    CUIStatic* m_center{};
    CUIStatic* m_compass{};
    CUIStatic* m_clipFrame{};
    CUIStatic* m_pointerDistanceText{};
    CUIStatic* m_Counter{};
    CUIStatic* m_Counter_text{};
    CUIStatic* m_clock_wnd{};
    CUIStatic* m_clock_wnd_background{};
    u8 m_current_map_idx{ u8(-1) };

    bool m_rounded{};
    bool m_use_cop_sizes{};
    u32 m_alpha{};

public:
    CUIZoneMap();
    virtual ~CUIZoneMap();

    void Init();

    void Render();
    void Update();

    bool ZoomIn();
    bool ZoomOut();

    void ApplyZoom();

    CUIStatic* Background() const { return m_background; };
    CUIWindow* MapFrame() const { return m_clipFrame; };
    void SetupCurrentMap();
    void OnSectorChanged(IRender_Sector::sector_id_t sector);
    void Counter_ResetClrAnimation();

private:
    void SetHeading(float angle);
    void UpdateRadar(Fvector pos);
};
