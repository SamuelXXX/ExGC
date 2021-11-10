#ifndef __EXGC_H__
#define __EXGC_H__

#include <iostream>
#include <string>

#include"ExGCCore.h"
#include"ExGCObject.h"
#include"ExGCPool.h"
#include"ExGCPoolVisitor.h"
#include"ExGCRefType.h"

#define ExTractHeaderPtr(ob_ptr) ((GCPoolHeader *)((uint8_t *)ob_ptr-sizeof(GCPoolHeader)))
#define ExTractObjectPtr(header_ptr) ((GCObject *)((uint8_t *)header_ptr+sizeof(GCPoolHeader)))
#define InvalidGenID 99

namespace exgc
{
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

    void ProfileGeneration(int);
    void Profile();
    void CollectGeneration(int);
    void Collect();
}

#endif