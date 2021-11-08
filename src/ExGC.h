#ifndef __EXGC_H__
#define __EXGC_H__

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <vector>
#include <iostream>

namespace exgc
{
    class GCPoolManager;
    class GCObject;
    template <class T>
    class Ref;

    struct GCPoolHeader // ***Defination Complete
    {
        GCPoolManager *owner;
        uint32_t temp_refcnt;
        GCObject *m_item;
    };

    class GCPoolManager
    {
        GCPoolHeader *head;
        GCPoolHeader *tail;
        GCPoolHeader *current;

    public:
        GCPoolManager() = delete;
        GCPoolManager(uint32_t);

    public:
        void Push(GCObject *);    // Push a GCObject into pool to manage this object
        void Kick(GCObject *); // Disconnect GCObject to this pool
        GCObject *Pop(); // Pop tail GCObject before free or transfer out
        void CollectPool(); // Collect all cycle reference inside this pool

    public:
        inline bool Contain(GCPoolHeader *header)
        {
            return header >= this->head && header < this->current;
        }

        inline bool IsEmpty()
        {
            return current == head;
        }

        inline bool IsFull()
        {
            return current == tail;
        }

        inline size_t Size()
        {
            return static_cast<size_t>(current-head);
        }

        inline size_t ReservedSize()
        {
            return static_cast<size_t>(tail-current);
        }
    };

    class GCGenerationManager final
    {
        GCPoolManager m_gen1;
        GCPoolManager m_gen2;
        GCPoolManager m_gen3;

    private:
        GCGenerationManager();
        GCGenerationManager(const GCGenerationManager &) = delete;
        void add(GCObject *);

    public:
        static GCGenerationManager *GetInstance();
        void Collect(int);

        friend class GCObject;
    };

    class GCObject // ***Defination Complete
    {
        GCPoolHeader *m_header;
        uint32_t m_refcnt;
        uint32_t test_number;
        public:
        inline void DecTempRefcnt()
        {
            m_header->temp_refcnt--;
        }
        

    public:
        void *operator new(std::size_t);
        void operator delete(void *ptr); // Not allowed to delete GCObject Manually  

    public:
        virtual void GCTrackReference();
        static void GCIncRef(GCObject *ptr);
        static void GCDecRef(GCObject *ptr);

    public:
        GCObject()=default; //Do Nothing

        friend class GCPoolManager;
        friend void transfer(GCPoolHeader *, GCPoolHeader *);
    };

    inline void transfer(GCPoolHeader *dst, GCPoolHeader *src)
    {
        dst->temp_refcnt=src->temp_refcnt;
        dst->m_item=src->m_item;
        dst->m_item->m_header=dst;
    }

    class IReferenceType
    {
    public:
        virtual void Resolve() = 0;
    };
    template <class T>
    class Ref final : IReferenceType // ** Defination Complete
    {
        T *m_object_ptr;

    public:
        Ref<T>()
        {
            m_object_ptr = nullptr;
        }
        // Direct Construct, Only Available in GC_New function.
        Ref<T>(T *ptr)
        {
            m_object_ptr = ptr;

            if (m_object_ptr != nullptr)
            {
                GCObject::GCIncRef(m_object_ptr);
            }
        }
        //Copy Construct
        Ref<T>(const Ref<T> &other)
        {
            m_object_ptr = other.m_object_ptr;

            if (m_object_ptr != nullptr)
                GCObject::GCIncRef(m_object_ptr);
        }
        // Move Construct
        Ref<T>(Ref<T> &&other)
        {
            m_object_ptr = other.m_object_ptr;
            other.m_object_ptr = nullptr;
        }
        //Destructor
        ~Ref<T>()
        {
            if (m_object_ptr != nullptr)
                GCObject::GCDecRef(m_object_ptr);
            m_object_ptr = nullptr;
        }

    public:
        // Copy Assign
        Ref<T> &operator=(T *ptr)
        {
            if (m_object_ptr != nullptr)
                GCObject::GCDecRef(m_object_ptr);

            m_object_ptr = ptr;

            if (m_object_ptr != nullptr)
                GCObject::GCIncRef(m_object_ptr);

            return *this;
        }

        // Copy Assign
        Ref<T> &operator=(const Ref<T> &other)
        {
            if (m_object_ptr != nullptr)
                GCObject::GCDecRef(m_object_ptr);

            m_object_ptr = other.m_object_ptr;

            if (m_object_ptr != nullptr)
                GCObject::GCIncRef(m_object_ptr);

            return *this;
        }

        // Move Assign
        Ref<T> &operator=(Ref<T> &&other)
        {
            if (m_object_ptr != nullptr)
                GCObject::GCDecRef(m_object_ptr);

            m_object_ptr = other.m_object_ptr;
            other.m_object_ptr = nullptr;

            return *this;
        }

        // Pointer Operation
        T *operator->()
        {
            return m_object_ptr;
        }

    public:
        void Resolve() override
        {
            if (m_object_ptr != nullptr)
            {
                m_object_ptr->DecTempRefcnt();
            }
        }

        friend class GCObject;
    };
}

#endif