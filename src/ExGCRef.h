#ifndef __EXGCREF_H__
#define __EXGCREF_H__

#include "ExGCPool.h"

namespace exgc
{
    template <class T>
    class Ref final
    {
    public:
        GCPoolItem<T> *m_ref_item;
        T *m_object_ptr;

        Ref<T>()
        {
            m_ref_item = nullptr;
            m_object_ptr = nullptr;
        }
        // Direct Construct, Only Available in GC_New function.
        Ref<T>(T *ptr)
        {
            m_ref_item = ptr;

            if (m_ref_item != nullptr)
            {
                GCIncRef(m_ref_item);
                m_object_ptr = &(m_ref_item->m_object)
            }
            else
            {
                m_object_ptr = nullptr;
            }
        }
        //Copy Construct
        Ref<T>(const Ref<T> &other)
        {
            m_ref_item = other.m_ref_item;
            m_object_ptr = other.m_object_ptr;

            if (m_ref_item != nullptr)
                GCIncRef(m_ref_item);
        }
        // Move Construct
        Ref<T>(Ref<T> &&other)
        {
            m_ref_item = other.m_ref_item;
            m_object_ptr = other.m_object_ptr;
            other.m_ref_item = nullptr;
            other.m_object_ptr = nullptr;
        }
        //Destructor
        ~Ref<T>()
        {
            if (m_ref_item != nullptr)
                GCDecRef(m_ref_item);
            m_ref_item = nullptr;
        }

    public:
        // Copy Assign
        Ref<T> &operator=(const Ref<T> &other)
        {
            if (m_ref_item != nullptr)
                GCDecRef(m_ref_item);

            m_ref_item = other.m_ref_item;
            m_object_ptr = other.m_object_ptr;

            if (m_ref_item != nullptr)
                GCIncRef(m_ref_item);

            return *this;
        }

        // Move Assign
        Ref<T> &operator=(Ref<T> &&other)
        {
            if (m_ref_item != nullptr)
                GCDecRef(m_ref_item);

            m_ref_item = other.m_ref_item;
            m_object_ptr = other.m_object_ptr;

            other.m_ref_item = nullptr;
            other.m_object_ptr = nullptr;

            return *this;
        }

        // Pointer Operation
        T *operator->()
        {
            return m_object_ptr;
        }

        template <typename... Args>
        static Ref<T> New(Args... args);
    };

    template <class T>
    template <typename... Args>
    Ref<T> Ref<T>::New(Args... args)
    {
        std::allocator<GCPoolItem<T>> pool_alloc;
        std::allocator<T> elem_alloc;

        // Create object and initialize values
        T *pool_item_ptr = pool_alloc.allocate(1);
        pool_item_ptr->m_refcnt = 0;
        pool_item_ptr->m_header = nullptr;
        elem_alloc.construct(&(pool_item_ptr->m_object), Args... args);

        generation1.AddItem(pool_item_ptr);

        return Ref<T>(pool_item_ptr);
    }
}

#endif