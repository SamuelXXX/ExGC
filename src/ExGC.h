#ifndef __EXGC_H__
#define __EXGC_H__

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <vector>
#include <iostream>
#include <string>

namespace exgc
{
    class GCPoolManager;
    class GCObject;
    template <class T>
    class Ref;

    inline void GCLog(GCObject *obj,const char * c_str)
    {
        if(obj==nullptr)
        {
            std::cout<<"[GCLOG] >>>>>>>"<<c_str<<std::endl;
        }
        else
        {
            std::cout<<"[GCLOG] >>>>>>>"<<obj<<":"<<c_str<<std::endl;
        }
        
    }

    inline void GCLog(GCObject *obj,const std::string& str)
    {
        GCLog(obj, str.c_str());
    }

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
        inline size_t HeaderPoolMemory()
        {
            size_t total_size=0;
            total_size+=Capacity()*sizeof(GCPoolHeader);
            return total_size;
        }

        inline size_t TotalMemory()
        {
            size_t total_size=0;
            total_size+=sizeof(*this);
            total_size+=Capacity()*sizeof(GCPoolHeader);
            return total_size;
        }

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

        inline size_t Capacity()
        {
            return static_cast<size_t>(tail-head);
        }
    
    public: // Profile interfaces
        void Profile();
    };

    class GCGenerationManager final
    {
        GCPoolManager m_gen1;
        GCPoolManager m_gen2;
        GCPoolManager m_gen3;
        size_t m_objectsmem;

    private:
        GCGenerationManager();
        GCGenerationManager(const GCGenerationManager &) = delete;
        void add(GCObject *);

    public:
        static GCGenerationManager *GetInstance();
        void Collect(int);
        
    public:
        void MemoryProfile()
        {
            GCLog(nullptr, "GCObjectMem:"+std::to_string(m_objectsmem));
            GCLog(nullptr, "Gen1_Mem:"+std::to_string(m_gen1.TotalMemory()));
            GCLog(nullptr, "Gen2_Mem:"+std::to_string(m_gen2.TotalMemory()));
            GCLog(nullptr, "Gen3_Mem:"+std::to_string(m_gen3.TotalMemory()));
        }

        void GenerationProfile(int index)
        {
            GCLog(nullptr, "GenProfile-"+std::to_string(index));
            switch(index)
            {
                case 1:m_gen1.Profile();break;
                case 2:m_gen2.Profile();break;
                case 3:m_gen3.Profile();break;
                default:break;
            }
            GCLog(nullptr, "EndGenProfile-"+std::to_string(index));
        }

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

    inline void Profile(int gen_index)
    {
        GCGenerationManager::GetInstance()->MemoryProfile();
        GCGenerationManager::GetInstance()->GenerationProfile(gen_index);
    }

    inline void Collect(int gen_index)
    {
        GCGenerationManager::GetInstance()->Collect(gen_index);
    }
}

#endif