#include<iostream>
#include"../src/ExGC.h"
using namespace std;
using namespace exgc;

class A:public GCObject
{
	Ref<B> m_ob;
};

class B:public GCObject
{
	Ref<A> m_ob;
};

int main()
{
	cout<<"Program bits:"<<sizeof(int*)*8<<endl;		
	getchar();
	return 0;
}