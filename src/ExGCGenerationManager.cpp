#include "ExGC.h"

namespace exgc
{
    static GCGenerationManager * _gc_manager=nullptr;

    GCGenerationManager *GCGenerationManager::GetInstance()
    {
        if(_gc_manager==nullptr)
        {
            _gc_manager=new GCGenerationManager();
        }

        return _gc_manager;
    }

    GCGenerationManager::GCGenerationManager():m_gen1(1024),m_gen2(2048),m_gen3(4096),m_objectsmem(0)
    {

    }

    void GCGenerationManager::add(GCObject *gc_ptr)
    {
        if(!m_gen1.IsFull())
        {
            m_gen1.Push(gc_ptr);
            return;
        }

        m_gen1.CollectPool();
        assert(!m_gen1.IsFull());
        m_gen1.Push(gc_ptr);

    }

    void GCGenerationManager::Collect(int gen_index)
    {
        m_gen1.CollectPool();
    }
}