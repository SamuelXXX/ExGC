#ifndef __EXGC_OBJECT_H__
#define __EXGC_OBJECT_H__
#include<vector>

namespace exgc
{

    class GCPoolItemBase;
    
    class GCObject
    {
        public:
        virtual void GetReferee(std::vector<GCPoolItemBase *>& v){}
    };
}

#endif