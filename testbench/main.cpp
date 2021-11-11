#include<iostream>
#include"ExGCTestbench.h"
using namespace std;
using namespace ExGC::Testbench;

int main()
{
	cout<<"Program bits:"<<sizeof(int*)*8<<endl;
	ReferenceCounter::Test();
	CircularReference::Test();
	AutoCollect::Test();
	// GameRuntime::Test();
	std::cout<<"All Unit Testing Passed!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
	getchar();
	return 0;
}