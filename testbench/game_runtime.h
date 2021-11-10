#ifndef __GAMERUNTIME_H__
#define __GAMERUNTIME_H__

#include "exgc_testbench.h"
#include<vector>

namespace ExGC::Testbench::GameRuntime
{
    template <class T>
    class Vector : public std::vector<Ref<T>>, public IGCReferenceType
    {
    public:
        void OnRefVisited(GCPoolVisitor &v) override
        {
            for (auto &ref : *this)
            {
                v.Visit(ref);
            }
        }
    };

    class Component;
    class Entity : public GCObject
    {
        char m_name[1024];
        Vector<Component> m_components;
    public:
        Entity(const char *name)
        {
            strcpy(m_name, name);
        }
        void PrintName()
        {
            std::cout << "Entity:" << m_name << std::endl;
        }
    public:
        void AddComponent(Ref<Component>);
        void RemoveComponent(Ref<Component>);

    public:
        void GCTrackReference(GCPoolVisitor &v) override;
    };


    class Component : public GCObject
    {
        Ref<Entity> m_owner;
    public:
        void SetOwner(Ref<Entity>);
    public:
        void GCTrackReference(GCPoolVisitor &v) override;
    };
    

    class Transform : public Component
    {
        float x, y, z;

    public:
        Transform(float x, float y, float z) : x(x), y(y), z(z) {}
    };
}

#endif