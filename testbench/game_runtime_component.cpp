#include"game_runtime.h"

namespace exgc::testbench::game_runtime
{
    void Component::SetOwner(Ref<Entity> owner)
    {
        m_owner = owner;
    }

    void Component::GCTrackReference(GCPoolVisitor &v)
    {
        v.Visit(m_owner);
    }
}