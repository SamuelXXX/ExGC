#include<iostream>
#include"../src/ExGC.h"
using namespace std;
using namespace exgc;

class B;
class A:public GCObject
{
	public:
	Ref<B> m_ob;
	void GCTrackReference() override
	{
		m_ob.Resolve();
	}
	
};

class B:public GCObject
{
	public:
	Ref<A> m_ob;
	void GCTrackReference() override
	{
		m_ob.Resolve();
	}
};

void Test()
{
	std::vector<Ref<A>> vec_of_ref_a;
	Ref<A> a=new A();
	Ref<B> b=new B();
	// a->m_ob=b;
	// b->m_ob=a;
}

int main()
{
	cout<<"Program bits:"<<sizeof(int*)*8<<endl;
	Test();
	GCGenerationManager::GetInstance()->Collect(1);
	getchar();
	return 0;
}