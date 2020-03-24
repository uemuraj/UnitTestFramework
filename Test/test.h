//*****************************************************************************
//
// Description : テストツール自体を開発するテスト
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

// ※テストメソッドで "virtual" を指定しないと、直接呼び出されていないため関数が実行モジュールから省かれてしまうことに注意

//
// 各種 assert() で例外が発生しないことを確認
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
// 各種 assert() で例外が発生することを確認
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
// ユーザ定義例外・構造化例外が処理できることを確認
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTest3 : public TestCase<FrameworkTest3>
{
public:
	virtual void testUserDefinedException();
	virtual void testAccessViolation();
};

//
// setUp() / testXxxx() / tearDown() の呼出し順が守られることの確認
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
// テストに失敗しても setUp() / testXxxx() / tearDown() の呼出し順が守られることの確認
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
// tearDown() の呼出し中に例外が発生しても処理できることの確認
//
///////////////////////////////////////////////////////////////////////////////

class FrameworkTest4 : public TestCase<FrameworkTest4>
{
public:
	virtual void testXxxx();
	virtual void tearDown();
};

//
// テストがすべて失敗していることを確認するためのアダプタ(?)
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