#include <vector>
#include "exgc_testbench.h"

namespace exgc::testbench::custom_container
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

    public:
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

    void Entity::AddComponent(Ref<Component> component)
    {
        component->SetOwner(this);
        m_components.push_back(component);
    }

    void Entity::RemoveComponent(Ref<Component> component)
    {
        for (auto it = m_components.begin(); it != m_components.end();)
        {
            if (*it == component)
            {
                it = m_components.erase(it);
                component->SetOwner(nullptr);
            }
            else
                ++it;
        }
    }

    void Entity::GCTrackReference(GCPoolVisitor &v)
    {
        v.Visit(m_components);
    }

    void Component::SetOwner(Ref<Entity> owner)
    {
        m_owner = owner;
    }

    void Component::GCTrackReference(GCPoolVisitor &v)
    {
        v.Visit(m_owner);
    }

    void unit_test(int index)
    {
        Ref<Entity> entity = new Entity(("SamaelXXX" + std::to_string(index)).c_str());
        entity->AddComponent(new Transform(1.f, 1.f, 1.f));
        // entity->PrintName();
    }

    bool Test()
    {
        for (int i = 0; i < 1000; i++)
            unit_test(i);
        exgc::Profile();
        exgc::Collect();
        exgc::Profile();
        return GCSizeCondition(1, 0);
    }
}