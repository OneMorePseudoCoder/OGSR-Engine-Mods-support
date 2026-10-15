#pragma once

#include "gameobject.h"
#include "physicsshellholder.h"
#include "physicsskeletonobject.h"
#include "PHSkeleton.h"
#include "script_export_space.h"

class CSE_ALifeObjectPhysic;
class CPhysicsElement;

class CPhysicObject : public CPhysicsShellHolder, public CPHSkeleton
{
    typedef CPhysicsShellHolder inherited;
    EPOType m_type;
    float m_mass;
    ICollisionHitCallback* m_collision_hit_callback;
    bool m_is_ai_obstacle;

private:
    // Creating
    void CreateBody(CSE_ALifeObjectPhysic* po);
    void CreateSkeleton(CSE_ALifeObjectPhysic* po);
    void AddElement(CPhysicsElement* root_e, int id);

public:
	bool get_door_vectors(Fvector& closed, Fvector& open) const;

public:
    CPhysicObject(void);
    virtual ~CPhysicObject(void);

    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void CreatePhysicsShell(CSE_Abstract* e);
    virtual void net_Destroy();
    virtual void Load(LPCSTR section);
    virtual void shedule_Update(u32 dt);
    virtual void UpdateCL();
    virtual void net_Save(NET_Packet& P);
    virtual BOOL net_SaveRelevant();
    virtual BOOL UsedAI_Locations();
    virtual ICollisionHitCallback* get_collision_hit_callback();
    virtual void set_collision_hit_callback(ICollisionHitCallback* cc);
    virtual bool is_ai_obstacle() const;
    void set_ai_obstacle(bool flag) { m_is_ai_obstacle = flag; }

protected:
    virtual void SpawnInitPhysics(CSE_Abstract* D);
    virtual void RunStartupAnim(CSE_Abstract* D);
    virtual CPhysicsShellHolder* PPhysicsShellHolder() { return PhysicsShellHolder(); }
    virtual CPHSkeleton* PHSkeleton() { return this; }
    virtual void InitServerObject(CSE_Abstract* po);
    virtual void PHObjectPositionUpdate();

    bool m_just_after_spawn;
    bool m_activated;

    DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CPhysicObject)
#undef script_type_list
#define script_type_list save_type_list(CPhysicObject)
