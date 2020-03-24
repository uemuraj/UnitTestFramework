//*****************************************************************************
//
// Description : �e�X�g�c�[�����̂��J������e�X�g
//
// Environment : Visual C++ 6.0
//
//*****************************************************************************
//
// $Id: test.h,v 1.2 2004/04/06 12:12:07 uemuraj Exp $
//
//*****************************************************************************

#ifndef TEST_H
#define TEST_H

// ���e�X�g���\�b�h�� "virtual" ���w�肵�Ȃ��ƁA���ڌĂяo����Ă��Ȃ����ߊ֐������s���W���[������Ȃ���Ă��܂����Ƃɒ���

//
// �e�� assert() �ŗ�O���������Ȃ����Ƃ��m�F
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTest1 : public TestCase<FrameworkTest1>
{
public:
	virtual void testNoAssert();
	virtual void testAssertTrue();
	virtual void testAssertFalse();
	virtual void testAssertNull();
	virtual void testAssertNotNull();
	virtual void testAssertEqualsZ1();
	virtual void testAssertEqualsZ2();
	virtual void testAssertEqualsJ();
	virtual void testAssertEqualsV();
	virtual void testAssertEqualsC();
};

//
// �e�� assert() �ŗ�O���������邱�Ƃ��m�F
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTest2 : public TestCase<FrameworkTest2>
{
public:
	virtual void testForceFail();
	virtual void testAssertTrue();
	virtual void testAssertFalse();
	virtual void testAssertNull();
	virtual void testAssertNotNull();
	virtual void testAssertEqualsZ1();
	virtual void testAssertEqualsZ2();
	virtual void testAssertEqualsJ();
	virtual void testAssertEqualsV();
	virtual void testAssertEqualsC();
};

//
// ���[�U��`��O�E�\������O�������ł��邱�Ƃ��m�F
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTest3 : public TestCase<FrameworkTest3>
{
public:
	virtual void testUserDefinedException();
	virtual void testAccessViolation();
};

//
// setUp() / testXxxx() / tearDown() �̌ďo����������邱�Ƃ̊m�F
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTest6 : public TestCase<FrameworkTest6>
{
public:
	FrameworkTest6();
	~FrameworkTest6();
	virtual void setUp();
	virtual void testXxxx();
	virtual void tearDown();

private:
	int up, run, down;
};

//
// �e�X�g�Ɏ��s���Ă� setUp() / testXxxx() / tearDown() �̌ďo����������邱�Ƃ̊m�F
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTest5 : public TestCase<FrameworkTest5>
{
public:
	FrameworkTest5();
	~FrameworkTest5();
	virtual void setUp();
	virtual void testXxxx();
	virtual void tearDown();

private:
	int up, run, down;
};

//
// tearDown() �̌ďo�����ɗ�O���������Ă������ł��邱�Ƃ̊m�F
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTest4 : public TestCase<FrameworkTest4>
{
public:
	virtual void testXxxx();
	virtual void tearDown();
};

//
// �e�X�g�����ׂĎ��s���Ă��邱�Ƃ��m�F���邽�߂̃A�_�v�^(?)
//
///////////////////////////////////////////////////////////////////////////////

class TestCaseAdapter : public TestCase<TestCaseAdapter>
{
public:
	TestCaseAdapter(BaseTestCase &t, int r, int f, int e) : testCase(t), run(r), failure(f), error(e)
	{
	}

	void testCases();

private:
	BaseTestCase &testCase;

	int run;
	int error;
	int failure;
};

#endif//TEST_H