//*****************************************************************************
//
// Description : �e�X�g�c�[�����̂��J������e�X�g
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
// �e�� assert() �ŗ�O���������Ȃ����Ƃ��m�F
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
// �e�� assert() �ŗ�O���������邱�Ƃ��m�F
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
// ���[�U��`��O�E�\������O�������ł��邱�Ƃ��m�F
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
// tearDown() �̌ďo�����ɗ�O���������Ă������ł��邱�Ƃ̊m�F
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
// �e�X�g�Ɏ��s���Ă� setUp() / testXxxx() / tearDown() �̌ďo����������邱�Ƃ̊m�F
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
// setUp() / testXxxx() / tearDown() �̌ďo����������邱�Ƃ̊m�F
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
// �e�X�g�����ׂĎ��s���Ă��邱�Ƃ��m�F���邽�߂̃A�_�v�^
//
///////////////////////////////////////////////////////////////////////////////

#include <typeinfo.h>

void TestCaseAdapter::testCases()
{
	// �w�b�_�o��

	int count = printf("----- %s --------------------------------------------------", testCase.getClass().name());

	printf("\n\n");

	// �e�X�g�����s

	TestRunner runner(stdout);

	runner.run(testCase);

	assertEquals(runner.runCount(), run);
	assertEquals(runner.errorCount(), error);
	assertEquals(runner.failureCount(), failure);

	// �t�b�^�o��

	while (count-- > 0) putc('-', stdout);

	printf("\n\n");
}
