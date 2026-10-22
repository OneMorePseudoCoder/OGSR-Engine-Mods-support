////////////////////////////////////////////////////////////////////////////
//	Module 		: autosave_manager.h
//	Created 	: 04.11.2004
//  Modified 	: 09.11.2025
//	Author		: Dmitriy Iassenev
//	Description : Autosave manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAutosaveManager : public ISheduled
{
private:
    using inherited = ISheduled;

private:
    u32 m_autosave_interval;
    u32 m_last_autosave_time;
    u32 m_delay_autosave_interval;
    u32 m_not_ready_count;

public:
    CAutosaveManager();
    virtual ~CAutosaveManager();

    shared_str shedule_Name() const override { return {"autosave_manager"}; }
    void shedule_Update(u32 dt) override;
    float shedule_Scale() override;
    bool shedule_Needed() override { return true; }

    void on_game_loaded();

public:
    IC u32 autosave_interval() const { return (m_autosave_interval); }
    IC u32 last_autosave_time() const { return (m_last_autosave_time); }

    IC void update_autosave_time() { m_last_autosave_time = Device.dwTimeGlobal; }

    IC void delay_autosave() { m_last_autosave_time += m_delay_autosave_interval; }

    IC u32 not_ready_count() const { return (m_not_ready_count); }

    IC void inc_not_ready() { ++m_not_ready_count; }

    IC void dec_not_ready()
    {
        VERIFY(m_not_ready_count);
        --m_not_ready_count;
    }

    IC bool ready_for_autosave() { return (!m_not_ready_count); }
};
