#pragma once

class CPHReqBase;
class CPHReqComparerV;

#include "../xrPhysics/IPHWorld.h"
class CPhysicsShell;

class CPHReqBase
{
public:
    virtual ~CPHReqBase() {}
    virtual bool obsolete() const = 0;
    virtual bool compare(const CPHReqComparerV* v) const { return false; };
};

class CPHCondition : public CPHReqBase
{
public:
    virtual bool is_true() = 0;
};

class CPHAction : public CPHReqBase
{
public:
    virtual void run() = 0;
};

class CPHOnesCondition : public CPHCondition
{
    bool b_called;

public:
    CPHOnesCondition() { b_called = false; }
    virtual bool is_true()
    {
        b_called = true;
        return true;
    }
    virtual bool obsolete() const { return b_called; }
};

class CPHDummiAction : public CPHAction
{
public:
    virtual void run() { ; }
    virtual bool obsolete() const { return false; }
};

class CPHCall
{
    CPHAction* m_action;
    CPHCondition* m_condition;

public:
    CPHCall(CPHCondition* condition, CPHAction* action);
    ~CPHCall();
    void check();
    bool obsolete();
    bool equal(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    bool is_any(CPHReqComparerV* v);
#ifdef DEBUG
    const CPHAction* action() const { return m_action; }
    const CPHCondition* condition() const { return m_condition; }
#endif
};

using PHCALL_STORAGE = xr_vector<CPHCall*>;
using PHCALL_I = PHCALL_STORAGE::iterator;

class CPHCommander : public IPHWorldUpdateCallbck
{
    xrCriticalSection lock;
    PHCALL_STORAGE m_calls;

public:
    ~CPHCommander();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool add_call_unique(CPHCondition* condition, CPHReqComparerV* cmp_condition, CPHAction* action, CPHReqComparerV* cmp_action);
    void add_call(CPHCondition* condition, CPHAction* action);
    void add_call_threadsafety(CPHCondition* condition, CPHAction* action);

    void remove_call(PHCALL_I i);
    bool has_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    PHCALL_I find_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    void remove_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    void remove_calls(CPHReqComparerV* cmp_object);
    void remove_calls_threadsafety(CPHReqComparerV* cmp_object);

    void update();
    void update_threadsafety();

    void clear();

private:
    virtual void update_step() { update_threadsafety(); }
    virtual void phys_shell_relcase(CPhysicsShell* sh);
};

//-------------- SIMP: Вытащил скриптовый в отдельный класс, т.к. к физике он вообще не имеет отношения -------------------
class CPHCommanderScripts
{
public:
    class CPHCallScript
    {
        CPHAction* m_action{};
        CPHCondition* m_condition{};
        bool removed{};
        u32 paused{};

    public:
        CPHCallScript(CPHCondition* condition, CPHAction* action) : m_action(action), m_condition(condition) {}
        ~CPHCallScript()
        {
            xr_delete(m_action);
            xr_delete(m_condition);
        }

        void check()
        {
            if (m_condition && m_condition->is_true() && m_action)
                m_action->run();
        }
        bool obsolete() const { return !m_action || m_action->obsolete() || !m_condition || m_condition->obsolete(); }
        bool equal(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action) { return m_action->compare(cmp_action) && m_condition->compare(cmp_condition); }
        bool is_any(CPHReqComparerV* v) { return m_action->compare(v) || m_condition->compare(v); }

        bool isPaused() { return paused > Device.dwTimeGlobal; }
        bool isNeedRemove() const { return removed; }
        void removeLater() { removed = true; }
        void setPause(u32 ms) { paused = Device.dwTimeGlobal + ms; }
    };

private:
    using PHCALL_STORAGE = xr_vector<std::unique_ptr<CPHCallScript>>;
    using PHCALL_I = PHCALL_STORAGE::iterator;

    PHCALL_STORAGE m_calls;

public:
    ~CPHCommanderScripts() = default;

    CPHCallScript* add_call(CPHCondition* condition, CPHAction* action) { return m_calls.emplace_back(std::make_unique<CPHCallScript>(condition, action)).get(); }

    CPHCallScript* add_call_unique(CPHCondition* condition, CPHReqComparerV* cmp_condition, CPHAction* action, CPHReqComparerV* cmp_action)
    {
        auto it = find_call(cmp_condition, cmp_action);
        if (it == m_calls.end())
            return add_call(condition, action);
        return it->get();
    }

    PHCALL_I find_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action)
    {
        return std::find_if(m_calls.begin(), m_calls.end(), [&](auto& call) { return !call->isNeedRemove() && call->equal(cmp_condition, cmp_action); });
    }

    void remove_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action)
    {
        auto it = find_call(cmp_condition, cmp_action);
        if (it != m_calls.end())
        {
            auto call = it->get();
            call->removeLater();
        }
    }

    void remove_calls(CPHReqComparerV* cmp_object)
    {
        for (const auto& call : m_calls)
        {
            if (!call->isNeedRemove() && call->is_any(cmp_object))
            {
                call->removeLater();
            }
        }
    }

    void update()
    {
        for (const auto& call : m_calls)
        {
            if (!call->isNeedRemove() && !call->isPaused())
                call->check();
        }

        m_calls.erase(std::remove_if(m_calls.begin(), m_calls.end(), [](auto& call) { return call->isNeedRemove() || call->obsolete(); }), m_calls.end());
    }

    void clear() { m_calls.clear(); }
};
//-------------------------------------------------------------------------------------------------------------