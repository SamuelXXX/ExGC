#ifndef __EXGC_REF_TYPE_H__
#define __EXGC_REF_TYPE_H__

namespace ExGC
{
    // inline void GCLog(GCObject *obj,const char * c_str);
    class GCPoolVisitor;
    class IGCReferenceType
    {
        public:
        virtual void OnRefVisited(GCPoolVisitor& v)=0;
    };

    template <class T>
    class Ref final:public IGCReferenceType  // ** Defination Complete
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
                GCCore::GetInstance()->GCIncRef(m_object_ptr);
            }
        }
        //Copy Construct
        Ref<T>(const Ref<T> &other)
        {
            m_object_ptr = other.m_object_ptr;

            if (m_object_ptr != nullptr)
                GCCore::GetInstance()->GCIncRef(m_object_ptr);
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
                GCCore::GetInstance()->GCDecRef(m_object_ptr);
            // GCLog(nullptr,"Destroy Ref<T>!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            m_object_ptr = nullptr;
        }

    public:
        // Direct Assign
        Ref<T> &operator=(T *ptr)
        {
            if (m_object_ptr != nullptr)
                GCCore::GetInstance()->GCDecRef(m_object_ptr);

            m_object_ptr = ptr;

            if (m_object_ptr != nullptr)
                GCCore::GetInstance()->GCIncRef(m_object_ptr);

            return *this;
        }

        // Copy Assign
        Ref<T> &operator=(const Ref<T> &other)
        {
            if (m_object_ptr != nullptr)
                GCCore::GetInstance()->GCDecRef(m_object_ptr);

            m_object_ptr = other.m_object_ptr;

            if (m_object_ptr != nullptr)
                GCCore::GetInstance()->GCIncRef(m_object_ptr);

            return *this;
        }

        // Move Assign
        Ref<T> &operator=(Ref<T> &&other)
        {
            if (m_object_ptr != nullptr)
                GCCore::GetInstance()->GCDecRef(m_object_ptr);

            m_object_ptr = other.m_object_ptr;
            other.m_object_ptr = nullptr;

            return *this;
        }

        // Pointer Operation
        T *operator->()
        {
            return m_object_ptr;
        }

        T &operator*()
        {
            return *(m_object_ptr);
        }

        // Equal Operation
        bool operator==(const Ref<T> &other)
        {
            return m_object_ptr==other.m_object_ptr;
        }

    public:
        void OnRefVisited(GCPoolVisitor& v) override
        {
            v._visit(m_object_ptr);
        }

        friend class GCObject;
        friend class GCPoolVisitor;
    };
}

#endif