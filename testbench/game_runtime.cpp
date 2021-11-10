#include <vector>
#include "exgc_testbench.h"
#include "game_runtime.h"

namespace exgc::testbench::game_runtime
{
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
        // exgc::Profile();
        exgc::Collect();
        exgc::Profile();
        AssertGCSize(1,0);
        AssertGCSize(2,0);
        AssertGCSize(3,0);
        return GCSizeCondition(1, 0);
    }
}