#include"GameRuntime.h"

namespace ExGC::Testbench::GameRuntime
{
    void Component::SetOwner(Ref<Entity> owner)
    {
        m_owner = owner;
    }

    BeginCaptureRef(Component)
        CaptureRef(m_owner)
    EndCaptureRef
}