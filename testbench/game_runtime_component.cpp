#include"game_runtime.h"

namespace exgc::testbench::game_runtime
{
    void Component::SetOwner(Ref<Entity> owner)
    {
        m_owner = owner;
    }

    BeginCaptureRef(Component)
        CaptureRef(m_owner)
    EndCaptureRef
}