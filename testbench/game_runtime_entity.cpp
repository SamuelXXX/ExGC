#include"game_runtime.h"

namespace exgc::testbench::game_runtime
{
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

    BeginCaptureRef(Entity)
        CaptureRef(m_components)
    EndCaptureRef
}