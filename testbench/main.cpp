#include<iostream>
#include"exgc_testbench.h"
using namespace std;
using namespace exgc;

int main()
{
	cout<<"Program bits:"<<sizeof(int*)*8<<endl;
	exgc::testbench::simple::Test();
	getchar();
	return 0;
}