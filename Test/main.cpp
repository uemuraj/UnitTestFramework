//*****************************************************************************
//
// Description : テストツール自体を開発するテスト
//
// Environment : Visual C++ 6.0
//
//*****************************************************************************
//
// $Id: main.cpp,v 1.2 2004/04/06 12:12:07 uemuraj Exp $
//
//*****************************************************************************

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "unit.h"
#include "test.h"

//
// FrameworkTests
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTests : public TestSuite
{
	FrameworkTest1 test1;
	FrameworkTest2 test2;
	FrameworkTest3 test3;
	FrameworkTest4 test4;
	FrameworkTest5 test5;
	FrameworkTest6 test6;

	TestCaseAdapter test1X;
	TestCaseAdapter test2X;
	TestCaseAdapter test3X;
	TestCaseAdapter test4X;
	TestCaseAdapter test5X;
	TestCaseAdapter test6X;

public:
	FrameworkTests()
		: test1X(test1, 10, 0, 0), test2X(test2, 10, 10, 0), test3X(test3, 2, 0, 2), test4X(test4, 1, 1, 0), test5X(test5, 1, 1, 0), test6X(test6, 1, 0, 0)
	{
		addTestCase(test1X);
		addTestCase(test2X);
		addTestCase(test3X);
		addTestCase(test4X);
		addTestCase(test5X);
		addTestCase(test6X);
	}
};

//
// main
//
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	// テストツールそのものをテストする
	// コード 0 (0x0) で終了すれば、テストフレームワークは正しく動作している

	TestRunner runner(stdout);

	return runner.run(RepeatedTest(FrameworkTests(), 1));
}
