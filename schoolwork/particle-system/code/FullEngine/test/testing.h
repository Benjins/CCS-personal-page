#ifndef TESTING_H
#define TESTING_H

#ifdef TESTING

#include <string>
#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
#include <crtdbg.h>
#endif

using std::string; 

struct CrtCheckMemory
{
#if defined(_WIN32) || defined(_WIN64)
  _CrtMemState state1;
  _CrtMemState state2;
  _CrtMemState state3;
#endif

  CrtCheckMemory();
  ~CrtCheckMemory();
};

static int testCount = 0;
static int passedTests = 0;

int RunAllTests();

template<typename T>
void AssertEqual(T actual, T expected, string error){
	testCount++;

	if(actual == expected){
		passedTests++;
	}
	else{
		cerr << "Test Failed!  Msg: " << error << endl;
	}
}

void AssertTrue(bool check, string error);

void AssertApprox(float expected, float actual, string error, float maxDifference = 0.0001f);

double PerformanceTest(void (*func) (void), int count, string name);

static void BoxBoxPerformance();
static void BoxSpherePerformance();
static void SphereSpherePerformance();
static void MatrixMultPerformance();
static void SeparateAxisPerformance();


#endif

#endif