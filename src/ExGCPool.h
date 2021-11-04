#ifndef __EXGCPOOL_H__
#define __EXGCPOOL_H__

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <vector>

namespace exgc
{
    class ExGCGenerationPool;
    class GCPoolItemBase;

    struct GCPoolHeader
    {
        ExGCGenerationPool *owner;
        uint32_t temp_refcnt;
        GCPoolItemBase *m_item;
    };

    class GCPoolItemBase
    {
    public:
        GCPoolHeader *m_header;
        uint32_t m_refcnt;
        virtual void GetReferee(std::vector<GCPoolItemBase*>& v){};
        virtual ~GCPoolItemBase() = default;
    };

    template <class T>
    class GCPoolItem : public GCPoolItemBase
    {
    public:
        T m_object;
        GCPoolItem<T>() = delete;
        GCPoolItem<T>(const GCPoolItem<T> &) = delete;
        GCPoolItem<T>(GCPoolItem<T> &&) = delete;
        ~GCPoolItem<T>() = default;
        GCPoolItem<T> &operator=(const GCPoolItem<T> &) = delete;
        GCPoolItem<T> &operator=(GCPoolItem<T> &&) = delete;
        void GetReferee(std::vector<GCPoolItemBase*>& v) override
        {
            m_object.GetReferee(v);
        }
    };

    class ExGCGenerationPool
    {
        GCPoolHeader *head;
        GCPoolHeader *tail;
        GCPoolHeader *current;

    public:
        ExGCGenerationPool()=delete;
        ExGCGenerationPool(uint32_t);

    public:
        void AddItem(GCPoolItemBase *);
        void DestroyItem(GCPoolItemBase *);
        void Collect();
    };

    inline void GCIncRef(GCPoolItemBase *);
    inline void GCDecRef(GCPoolItemBase *);

    extern ExGCGenerationPool generation1;
}

#endif