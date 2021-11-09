#include<iostream>
#include"exgc_testbench.h"
#include<assert.h>
using namespace std;
using namespace exgc;

int main()
{
	cout<<"Program bits:"<<sizeof(int*)*8<<endl;
	assert(exgc::testbench::simple::Test());
	assert(exgc::testbench::circular_reference::Test());
	std::cout<<"All Unit Testing Passed!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
	getchar();
	return 0;
}