#include <vector>
#include "exgc_testbench.h"
#include "game_runtime.h"

namespace ExGC::Testbench::GameRuntime
{
    void unit_test(int index)
    {
        Ref<Entity> entity = new Entity(("SamaelXXX" + std::to_string(index)).c_str());
        entity->AddComponent(new Transform(1.f, 1.f, 1.f));
        // entity->PrintName();
    }

    void Test()
    {
        for (int i = 0; i < 1000; i++)
            unit_test(i);
        // exgc::Profile();
        ExGC::Collect();
        ExGC::Profile();
        AssertGCSize(0,0);
        AssertGCSize(1,0);
        AssertGCSize(2,0);
    }
}