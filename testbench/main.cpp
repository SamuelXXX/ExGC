#include<iostream>
#include"exgc_testbench.h"
#include<assert.h>
using namespace std;
using namespace ExGC;

int main()
{
	cout<<"Program bits:"<<sizeof(int*)*8<<endl;
	assert(ExGC::Testbench::Simple::Test());
	assert(ExGC::Testbench::CircularReference::Test());
	assert(ExGC::Testbench::GameRuntime::Test());
	std::cout<<"All Unit Testing Passed!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
	getchar();
	return 0;
}