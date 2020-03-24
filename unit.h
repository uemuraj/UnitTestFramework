//*****************************************************************************
//
// Description : ���j�b�g�e�X�g�@�\
//
// Environment : Visual C++ 6.0
//
//*****************************************************************************
//
// $Id: unit.h,v 1.2 2004/04/06 12:12:07 uemuraj Exp $
//
//*****************************************************************************

#ifndef UNIT_H
#define UNIT_H

//
// Assert �N���X
//
///////////////////////////////////////////////////////////////////////////////

class Assert
{
public:
	static void fail();
	static void assertTrue(bool condition);
	static void assertFalse(bool condition);
	static void assertNull(void *object);
	static void assertNotNull(void *object);
	static void assertEquals(bool expected, bool actual);
	static void assertEquals(const void *expected, const void *actual);
	static void assertEquals(const char *expected, const char *actual);
	static void assertEquals(signed int expected, signed int actual);
	static void assertEquals(signed long expected, signed long actual);
	static void assertEquals(unsigned int expected, unsigned int actual);
	static void assertEquals(unsigned long expected, unsigned long actual);
};

//
// Test �C���^�t�F�[�X
//
///////////////////////////////////////////////////////////////////////////////

class TestResult;

class Test
{
public:
	Test();
	virtual ~Test();
	virtual Test *addTest(Test *test);
	virtual Test *nextTest();
	virtual void run(TestResult &result) = 0;
private:
	Test *tests;
};

//
// TestCase �e���v���[�g
//
///////////////////////////////////////////////////////////////////////////////

class BaseTestCase : public Test
{
public:
	BaseTestCase();
	~BaseTestCase();
	virtual void setUp();
	virtual void tearDown();
	virtual void run(TestResult &result);
	virtual void run(TestResult &result, void (Test::*testMethod)());
	virtual const type_info &getClass() = 0;
};

template <class A> class TestCase : public Assert, public BaseTestCase
{
public:
	const type_info &getClass()
	{
		return typeid(A);
	}
};

//
// TestSuite �N���X
//
///////////////////////////////////////////////////////////////////////////////

class TestSuite : public Test
{
public:
	TestSuite();
	~TestSuite();
	virtual void run(TestResult &result);
	virtual void addTestCase(BaseTestCase &testCase);
private:
	Test *tests;
};

//
// RepeatedTest �N���X
//
///////////////////////////////////////////////////////////////////////////////

class RepeatedTest : public Test
{
public:
	RepeatedTest(Test &t, int c) : test(t), count(c)
	{
	}

	virtual void run(TestResult &result)
	{
		for (int i = 0; i < count; i++) {

			test.run(result);
		}
	}

private:
	Test &test;
	const int count;
};

//
// TestResult �C���^�[�t�F�[�X
//
///////////////////////////////////////////////////////////////////////////////

class Throwable;

class TestResult
{
public:
	virtual void startTest(Test &test) = 0;
	virtual void endTest(Test &test, unsigned int time) = 0;
	virtual void addError(Test &test, unsigned int time, Throwable &throwable) = 0;
	virtual void addFailure(Test &test, unsigned int time, Throwable &throwable) = 0;
	virtual int failureCount() = 0;
	virtual int errorCount() = 0;
	virtual int runCount() = 0;
};

//
// TestRunner �N���X
//
///////////////////////////////////////////////////////////////////////////////

class TestRunner : public TestResult
{
public:
	TestRunner(FILE *fp);
	~TestRunner();
	int run(Test &test);
	void startTest(Test &test);
	void endTest(Test &test, unsigned int time);
	void addError(Test &test, unsigned int time, Throwable &throwable);
	void addFailure(Test &test, unsigned int time, Throwable &throwable);
	int failureCount();
	int errorCount();
	int runCount();

private:
	FILE *stream;
	unsigned int runtime;
	int runs;
	int errors;
	int failures;
};

#endif//UNIT_H
