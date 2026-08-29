#pragma once

// SIMP: CS/COP: эти интерфейсы по методам отличаются от тех что в тч у нас.

class IPhysicsGeometry
{
public:
    virtual void get_Box(Fmatrix& form, Fvector& sz) const = 0;
    virtual bool collide_fluids() const = 0;
};

class IPhysicsElement
{
public:
    virtual const Fmatrix& XFORM() const = 0;
    virtual void get_LinearVel(Fvector& velocity) const = 0;
    virtual void get_AngularVel(Fvector& velocity) const = 0;
    virtual void get_Box(Fvector& sz, Fvector& c) const = 0;
    virtual const Fvector& mass_Center() const = 0;
    virtual u16 numberOfGeoms() const = 0;
    virtual const IPhysicsGeometry* geometry(u16 i) const = 0;
};

class IPhysicsShell
{
public:
    virtual const Fmatrix& XFORM() const = 0;
    virtual const IPhysicsElement& Element(u16 index) const = 0;
    virtual u16 get_ElementsNumber() const = 0;
};

class IObjectPhysicsCollision
{
public:
    virtual const IPhysicsShell* physics_shell() const = 0;
    virtual const IPhysicsElement* physics_character() const = 0; // deprecated
};
