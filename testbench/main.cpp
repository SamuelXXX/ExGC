#include<iostream>
#include"exgc_testbench.h"
using namespace std;
using namespace ExGC;

int main()
{
	cout<<"Program bits:"<<sizeof(int*)*8<<endl;
	ExGC::Testbench::Simple::Test();
	ExGC::Testbench::CircularReference::Test();
	ExGC::Testbench::GameRuntime::Test();
	std::cout<<"All Unit Testing Passed!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
	getchar();
	return 0;
}