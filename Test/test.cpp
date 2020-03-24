//*****************************************************************************
//
// Description : テストツール自体を開発するテスト
//
// Environment : Visual C++ 6.0
//
//*****************************************************************************
//
// $Id: test.cpp,v 1.1.1.1 2004/03/24 02:00:03 uemuraj Exp $
//
//*****************************************************************************

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "unit.h"
#include "test.h"

//
// 各種 assert() で例外が発生しないことを確認
//
///////////////////////////////////////////////////////////////////////////////

void FrameworkTest1::testNoAssert()
{
	::Sleep(100);
}

void FrameworkTest1::testAssertTrue()
{
	assertTrue(true);
}

void FrameworkTest1::testAssertFalse()
{
	assertFalse(false);
}

void FrameworkTest1::testAssertNull()
{
	assertNull(NULL);
}

void FrameworkTest1::testAssertNotNull()
{
	assertNotNull((void *)-1);
}

void FrameworkTest1::testAssertEqualsZ1()
{
	assertEquals(true, true);
}

void FrameworkTest1::testAssertEqualsZ2()
{
	assertEquals(false, false);
}

void FrameworkTest1::testAssertEqualsJ()
{
	long expected = 1, actual = 1;

	assertEquals(expected, actual);
}

void FrameworkTest1::testAssertEqualsV()
{
	const void *expected = this, *actual = this;

	assertEquals(expected, actual);
}

void FrameworkTest1::testAssertEqualsC()
{
	const char *expected = "'Expected String'", *actual = "'Expected String'";

	assertEquals(expected, actual);
}

//
// 各種 assert() で例外が発生することを確認
//
///////////////////////////////////////////////////////////////////////////////

void FrameworkTest2::testForceFail()
{
	fail();
}

void FrameworkTest2::testAssertTrue()
{
	assertTrue(false);
}

void FrameworkTest2::testAssertFalse()
{
	assertFalse(true);
}

void FrameworkTest2::testAssertNull()
{
	assertNull((void *)-1);
}

void FrameworkTest2::testAssertNotNull()
{
	assertNotNull(NULL);
}

void FrameworkTest2::testAssertEqualsZ1()
{
	assertEquals(true, false);
}

void FrameworkTest2::testAssertEqualsZ2()
{
	assertEquals(false, true);
}

void FrameworkTest2::testAssertEqualsJ()
{
	long expected = 1, actual = 2;

	assertEquals(expected, actual);
}

void FrameworkTest2::testAssertEqualsV()
{
	const void *expected = this, *actual = NULL;

	assertEquals(expected, actual);
}

void FrameworkTest2::testAssertEqualsC()
{
	const char *expected = "'Expected String'", *actual = "'Not Expected String'";

	assertEquals(expected, actual);
}

//
// ユーザ定義例外・構造化例外が処理できることを確認
//
///////////////////////////////////////////////////////////////////////////////

void FrameworkTest3::testUserDefinedException()
{
	class UserDefinedException {};

	throw UserDefinedException();
}

void FrameworkTest3::testAccessViolation()
{
	int *p = NULL;

	*p = 0;
}

//
// tearDown() の呼出し中に例外が発生しても処理できることの確認
//
///////////////////////////////////////////////////////////////////////////////

void FrameworkTest4::testXxxx()
{
}

void FrameworkTest4::tearDown()
{
	fail();
}

//
// テストに失敗しても setUp() / testXxxx() / tearDown() の呼出し順が守られることの確認
//
///////////////////////////////////////////////////////////////////////////////

FrameworkTest5::FrameworkTest5() : up(0), down(0), run(0)
{
}

FrameworkTest5::~FrameworkTest5()
{
	assertTrue(run > 0);
	assertEquals(run, up);
	assertEquals(run, down);
}

void FrameworkTest5::setUp()
{
	up++;

	assertEquals(run, up - 1);
}

void FrameworkTest5::testXxxx()
{
	run++;

	fail();
}

void FrameworkTest5::tearDown()
{
	down++;

	assertEquals(run, down);
}

//
// setUp() / testXxxx() / tearDown() の呼出し順が守られることの確認
//
///////////////////////////////////////////////////////////////////////////////

FrameworkTest6::FrameworkTest6() : up(0), down(0), run(0)
{
}

FrameworkTest6::~FrameworkTest6()
{
	assertTrue(run > 0);
	assertEquals(run, up);
	assertEquals(run, down);
}

void FrameworkTest6::setUp()
{
	up++;

	assertEquals(run, up - 1);
}

void FrameworkTest6::testXxxx()
{
	run++;

	assertEquals(run, up);
}

void FrameworkTest6::tearDown()
{
	down++;

	assertEquals(run, down);
}

//
// テストがすべて失敗していることを確認するためのアダプタ
//
///////////////////////////////////////////////////////////////////////////////

#include <typeinfo.h>

void TestCaseAdapter::testCases()
{
	// ヘッダ出力

	int count = printf("----- %s --------------------------------------------------", testCase.getClass().name());

	printf("\n\n");

	// テストを実行

	TestRunner runner(stdout);

	runner.run(testCase);

	assertEquals(runner.runCount(), run);
	assertEquals(runner.errorCount(), error);
	assertEquals(runner.failureCount(), failure);

	// フッタ出力

	while (count-- > 0) putc('-', stdout);

	printf("\n\n");
}
