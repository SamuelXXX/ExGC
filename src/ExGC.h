#ifndef __EXGC_H__
#define __EXGC_H__

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <vector>

namespace exgc
{
    class GCPoolManager;
    class GCObject;
    template<class T>
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
        GCPoolVisitor m_visitor;

    public:
        GCPoolManager()=default;
        GCPoolManager(uint32_t);

    public:
        void AddObject(GCObject *);
        void RemoveObject(GCObject *);
        void Collect();
        inline bool Contain(GCPoolHeader *header)
        {
            return header>=this->head && header<this->current;
        }
    };

    class GCGenerationManager
    {
        GCPoolManager m_gen1;
        GCPoolManager m_gen2;
        GCPoolManager m_gen3;

        private:
        GCGenerationManager();
        GCGenerationManager(const GCGenerationManager&)=delete;

        public:
        static GCGenerationManager* GetInstance();
        void Add(GCObject *);
        void Remove(GCObject *);
    };

    class GCObject // ***Defination Complete
    {
        GCPoolHeader *m_header;
        uint32_t m_refcnt;

        public:
        void *operator new(std::size_t);
        void operator delete(void *ptr);

        public:
        virtual void TraceReference(GCPoolVisitor& v);
        static void GCIncRef(GCObject* ptr);
        static void GCDecRef(GCObject* ptr);

        public:
        GCObject(){}; //Do Nothing

        friend class GCPoolManager;
    };

    class GCPoolVisitor // ****Defination Complete
    {
        GCPoolManager *owner;
        public:
        GCPoolVisitor(GCPoolManager *owner):owner(owner){}

        public:
        template <class T>
        void Trace(const Ref<T>&);
    };

    template <class T>
    void GCPoolVisitor::Trace(const Ref<T>& ref)
    {
        GCPoolHeader *header=ref.m_object_ptr.m_header;
        if(owner->Contain(header))// Refered a object in same pool
            --header->temp_refcnt; 
    }

    template <class T>
    class Ref final // ** Defination Complete
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

        friend class GCPoolVisitor;
        friend class GCObject;
    };
}

#endif