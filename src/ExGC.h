#ifndef __EXGC_H__
#define __EXGC_H__

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <vector>
#include <iostream>
#include <string>

#define ExTractHeaderPtr(ob_ptr) ((GCPoolHeader *)((uint8_t *)ob_ptr-sizeof(GCPoolHeader)))
#define ExTractObjectPtr(header_ptr) ((GCObject *)((uint8_t *)header_ptr+sizeof(GCPoolHeader)))
#define InvalidGenID 99

namespace exgc
{
    class GCGenerationManager;
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

    struct TrackState
    {
        bool trackRoot;
        bool reachable;
    };

    struct GCPoolHeader // ***Defination Complete
    {
        uint8_t obGenId;
        GCPoolHeader *prev;
        GCPoolHeader *next;
        
        size_t obSize;
        union 
        {
            uint32_t extRefcnt;
            TrackState trackState;
        };
    };

    class GCPoolManager final
    {
        const uint8_t m_genId;
        size_t m_maxSize;
        
        GCPoolHeader *head;
        GCPoolHeader *tail;
        
        size_t m_currentSize; // Current size of link
        size_t m_currentMemory; // Current allocated GCObject memory size
    
    private: // Node Management
        GCPoolHeader *addNode(GCPoolHeader *); // Insert a header node to pool, return added node
        GCPoolHeader *delNode(GCPoolHeader *); // delete a header node to pool, return next node of deleted node

    public:
        GCPoolManager() = delete;
        GCPoolManager(uint8_t, size_t);

    public:
        size_t GetGCObjectMemory(); // Get allocated GCObjects mem size
        size_t CalcGCObjectMemory(); // Iterating all GCObjects and calculate memory size
        size_t GetSize(); // Current amount of GCObject allocated in this
        size_t CalcSize(); // Iterating all GCObjects and calculate link list size
        bool Contain(GCPoolHeader *); // Check if header in this generation
    
    public:
        void CollectPool(); // Collect all cycle reference inside this pool
        bool ShouldGC(); // Current size exceed m_maxSize
        
    
    public: // Profile interfaces
        void Profile();
    
    friend class GCGenerationManager;
    };

    class GCGenerationManager final
    {
        bool m_refCounterFlag;
        GCPoolManager m_wild;
        GCPoolManager m_gen1;
        GCPoolManager m_gen2;
        GCPoolManager m_gen3;

    private:
        GCGenerationManager();
        GCGenerationManager(const GCGenerationManager &) = delete;
        void makeWild(GCPoolHeader *); // Make target allocated object a wild pointer
        void makeManaged(GCPoolHeader *); // Make a wild pointer managed by gc
        void ascend(GCPoolHeader *); // Increase generation of target object
        void kick(GCPoolHeader *); // Kick target object from all generation pool

    public:
        static GCGenerationManager *GetInstance();
        void Collect(int);
        void GCIncRef(GCObject *);
        void GCDecRef(GCObject *);
        void ToggleReferenceCounter(bool flag);
        
    public: // Profiling Interface
        void MemoryProfile(); // Profiling Memory Occupation
        void GenerationProfile(int index); // Profiling Generation
        size_t GetGenerationSize(uint8_t genId);
        size_t GetGenerationMemory(uint8_t genId);

        friend class GCObject;
    };

    class GCPoolVisitor
    {
        
        uint8_t visitStrategy;
        uint8_t visitGenId;
        void doTraceDirectRef(GCObject *);
        void doTraceIndirectRef(GCObject *);
        
    public:
        enum VisitStrategy{TraceDirectRef=0, TraceIndirectRef};
        GCPoolVisitor(uint8_t strategy,uint8_t genId):visitStrategy(strategy),visitGenId(genId){};
        
        void Visit(GCObject *);

        template<class T>
        void Visit(const Ref<T>& ptr_ref)
        {
            this->Visit(ptr_ref.m_object_ptr);
        }
    };

    class GCObject // ***Defination Complete
    {
        uint32_t m_refcnt;

    public:
        void *operator new(std::size_t);
        void operator delete(void *ptr); // Call destructor, kick from pool and free space
        

    public:
        virtual void GCTrackReference(GCPoolVisitor&);

    public:
        void IncRef();
        void DecRef();
        void ResetRef();
        uint32_t GetRefCount();

    public:
        GCObject()=default; //Do Nothing
        virtual ~GCObject()=default;

        friend class GCPoolManager;
    };
    template <class T>
    class Ref final  // ** Defination Complete
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
                GCGenerationManager::GetInstance()->GCIncRef(m_object_ptr);
            }
        }
        //Copy Construct
        Ref<T>(const Ref<T> &other)
        {
            m_object_ptr = other.m_object_ptr;

            if (m_object_ptr != nullptr)
                GCGenerationManager::GetInstance()->GCIncRef(m_object_ptr);
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
                GCGenerationManager::GetInstance()->GCDecRef(m_object_ptr);
            // GCLog(nullptr,"Destroy Ref<T>!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            m_object_ptr = nullptr;
        }

    public:
        // Copy Assign
        Ref<T> &operator=(T *ptr)
        {
            if (m_object_ptr != nullptr)
                GCGenerationManager::GetInstance()->GCDecRef(m_object_ptr);

            m_object_ptr = ptr;

            if (m_object_ptr != nullptr)
                GCGenerationManager::GetInstance()->GCIncRef(m_object_ptr);

            return *this;
        }

        // Copy Assign
        Ref<T> &operator=(const Ref<T> &other)
        {
            if (m_object_ptr != nullptr)
                GCGenerationManager::GetInstance()->GCDecRef(m_object_ptr);

            m_object_ptr = other.m_object_ptr;

            if (m_object_ptr != nullptr)
                GCGenerationManager::GetInstance()->GCIncRef(m_object_ptr);

            return *this;
        }

        // Move Assign
        Ref<T> &operator=(Ref<T> &&other)
        {
            if (m_object_ptr != nullptr)
                GCGenerationManager::GetInstance()->GCDecRef(m_object_ptr);

            m_object_ptr = other.m_object_ptr;
            other.m_object_ptr = nullptr;

            return *this;
        }

        // Pointer Operation
        T *operator->()
        {
            return m_object_ptr;
        }

        friend class GCObject;
        friend class GCPoolVisitor;
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