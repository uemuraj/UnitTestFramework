//*****************************************************************************
//
// Description : ユニットテスト機能
//
// Environment : Visual C++ 6.0
//
//*****************************************************************************
//
// $Id: unit.cpp,v 1.2 2004/04/06 12:12:07 uemuraj Exp $
//
//*****************************************************************************

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "unit.h"

//
// スタックトレースに関する追加定義
//
///////////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "imagehlp")

#include <eh.h>
#include <imagehlp.h>
#include <typeinfo.h>

// シンボル名の最大長を仮定（コンパイラの警告 C4011 を参照）

#define MAX_SYMBOLNAME (247 + 1)

// 不定長の IMAGEHLP_SYMBOL 構造体に対して固定長の IMAGEHLP_SYMBOL_BUFF 構造体を定義

struct IMAGEHLP_SYMBOL_BUFF : IMAGEHLP_SYMBOL
{
	TCHAR Buffer[MAX_SYMBOLNAME];
};


//
// イメージヘルプ API の初期化と後処理
//
///////////////////////////////////////////////////////////////////////////////

class ImageHelp
{
public:
	ImageHelp()
	{
		// シンボルファイルの検索パスを組み立る
		// デフォルトはカレントディレクトリからだがモジュールの存在するディレクトリに変更する

		TCHAR searchPath[MAX_PATH * 4 + 1];

		if (::GetModuleFileName(NULL, searchPath, MAX_PATH) == 0) {

			return;
		}

		if (TCHAR *path = _tcsrchr(searchPath, '\\')) {

			_tcscpy(path, ";");
		}

		if (TCHAR *path = _tgetenv("_NT_SYMBOL_PATH")) {

			_tcsncat(searchPath, path, MAX_PATH);
			_tcscat(searchPath, ";");
		}

		if (TCHAR *path = _tgetenv("_NT_ALTERNATE_SYMBOL_PATH")) {

			_tcsncat(searchPath, path, MAX_PATH);
			_tcscat(searchPath, ";");
		}

		if (TCHAR *path = _tgetenv("SystemRoot")) {

			_tcsncat(searchPath, path, MAX_PATH);
			_tcscat(searchPath, ";");
		}

		// オプションの追加設定

		::SymSetOptions(::SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_OMAP_FIND_NEAREST);

		// イメージヘルプを初期化

		::SymInitialize(::GetCurrentProcess(), searchPath, TRUE);
	}

	~ImageHelp()
	{
		// イメージヘルプの後処理

		::SymCleanup(::GetCurrentProcess());
	}
};

// プロセスの開始終了時に初期化と後処理が自動的に行われるよう
// グローバルなインスタンスをひとつだけ確保しておく

static ImageHelp imageHelp;


//
// Throwable クラス
//
///////////////////////////////////////////////////////////////////////////////

class Throwable
{
public:
	Throwable(DWORD ebp, DWORD esp, DWORD eip);
	virtual bool nextStackElement();
	virtual void printStackTrace(FILE *stream);

protected:
	const char *getMethodName() { return Symbol.Name; }
	const char *getFileName() { return Line.FileName; }
	int getLineNumber() { return Line.LineNumber; }

private:
	HANDLE hProcess;
	HANDLE hThread;
	CONTEXT Context;
	STACKFRAME StackFrame;
	IMAGEHLP_SYMBOL_BUFF Symbol;
	IMAGEHLP_LINE Line;
};

Throwable::Throwable(DWORD ebp, DWORD esp, DWORD eip)
{
	// StackWalk() に必要なメンバ変数を初期化

	hProcess = ::GetCurrentProcess();
	hThread = ::GetCurrentThread();

	ZeroMemory(&Context, sizeof(Context));
	ZeroMemory(&StackFrame, sizeof(StackFrame));
	ZeroMemory(&Symbol, sizeof(Symbol));
	ZeroMemory(&Line, sizeof(Line));

	StackFrame.AddrPC.Mode = AddrModeFlat;
	StackFrame.AddrFrame.Mode = AddrModeFlat;
	StackFrame.AddrStack.Mode = AddrModeFlat;

	StackFrame.AddrPC.Offset = eip;
	StackFrame.AddrFrame.Offset = ebp;
	StackFrame.AddrStack.Offset = esp;

	Symbol.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	Symbol.MaxNameLength = MAX_SYMBOLNAME;

	Line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
}

bool Throwable::nextStackElement()
{
	// このフレームワークの詳細は出力させない

	if (_tcscmp("BaseTestCase::run", getMethodName()) == 0) {

		return false;
	}

	// 呼び出しスタックをたどる

	if (::StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &StackFrame, &Context, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL) && StackFrame.AddrFrame.Offset && StackFrame.AddrReturn.Offset) {

		DWORD dwAddress = StackFrame.AddrPC.Offset;

		// 実行アドレスからシンボル名を取得

		DWORD dwDisplacement = 0;

		if (!::SymGetSymFromAddr(hProcess, dwAddress, &dwDisplacement, &Symbol)) {

			// モジュール名と実行アドレスで代用

			IMAGEHLP_MODULE Module = { sizeof(Module) };

			if (::SymGetModuleInfo(hProcess, dwAddress, &Module)) {

				_sntprintf(Symbol.Name, MAX_SYMBOLNAME, "%s::<%08X>", Module.ModuleName, dwAddress);

			} else {

				return false; // _sntprintf(Symbol.Name, MAX_SYMBOLNAME, "%08X", dwAddress);
			}
		}

		// 実行アドレスから行番号を取得

		Line.LineNumber = 0;

		for (DWORD dwTempDisplacement = 0; dwTempDisplacement < 100; dwTempDisplacement++) {

			DWORD dwDisplacement = 0;

			if (::SymGetLineFromAddr(hProcess, dwAddress - dwTempDisplacement, &dwDisplacement, &Line)) {

				break;
			}
		}

		return true;
	}

	return false;
}

void Throwable::printStackTrace(FILE *stream)
{
	TCHAR MethodNameMask[MAX_SYMBOLNAME + 1] = "";

	int count = 0;

	// 再帰呼び出しは出力をたたむ

	if (_tcscmp(MethodNameMask, getMethodName()) == 0) {

		count++;
		return;

	} else {

		_tcscpy(MethodNameMask, getMethodName());

		if (count > 0) {

			_ftprintf(stream, "    ... %u more\n", count);
		}
	}

	count = 0;

	// 行番号の情報の有無でフォーマットを変える

	if (getLineNumber() > 0) {

		_ftprintf(stream, " at %s (%s:%u)\n", getMethodName(), getFileName(), getLineNumber());

	} else {

		_ftprintf(stream, " at %s\n", getMethodName());
	}
}


//
// StructuredException クラス
//
///////////////////////////////////////////////////////////////////////////////

class StructuredException : public Throwable
{
public:
	StructuredException(EXCEPTION_POINTERS *p, unsigned int c) : Throwable(p->ContextRecord->Ebp, p->ContextRecord->Esp, p->ContextRecord->Eip), code(c)
	{
	}

	void printStackTrace(FILE *stream)
	{
		// 例外の発生個所をタグジャンプ可能なフォーマットで出力

		if (nextStackElement()) {

			_ftprintf(stream, "%s(%d): StructuredException: %s\n", getFileName(), getLineNumber(), getMessage());

			do {
				Throwable::printStackTrace(stream);

			} while (nextStackElement());
		}
	}

	const char *getMessage()
	{
		switch (code) {

		case EXCEPTION_ACCESS_VIOLATION:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return "EXCEPTION_DATATYPE_MISALIGNMENT";
		case EXCEPTION_BREAKPOINT:
			return "EXCEPTION_BREAKPOINT";
		case EXCEPTION_SINGLE_STEP:
			return "EXCEPTION_SINGLE_STEP";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return "EXCEPTION_FLT_DENORMAL_OPERAND";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
		case EXCEPTION_FLT_INEXACT_RESULT:
			return "EXCEPTION_FLT_INEXACT_RESULT";
		case EXCEPTION_FLT_INVALID_OPERATION:
			return "EXCEPTION_FLT_INVALID_OPERATION";
		case EXCEPTION_FLT_OVERFLOW:
			return "EXCEPTION_FLT_OVERFLOW";
		case EXCEPTION_FLT_STACK_CHECK:
			return "EXCEPTION_FLT_STACK_CHECK";
		case EXCEPTION_FLT_UNDERFLOW:
			return "EXCEPTION_FLT_UNDERFLOW";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return "EXCEPTION_INT_DIVIDE_BY_ZERO";
		case EXCEPTION_INT_OVERFLOW:
			return "EXCEPTION_INT_OVERFLOW";
		case EXCEPTION_PRIV_INSTRUCTION:
			return "EXCEPTION_PRIV_INSTRUCTION";
		case EXCEPTION_IN_PAGE_ERROR:
			return "EXCEPTION_IN_PAGE_ERROR";
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return "EXCEPTION_ILLEGAL_INSTRUCTION";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
		case EXCEPTION_STACK_OVERFLOW:
			return "EXCEPTION_STACK_OVERFLOW";
		case EXCEPTION_INVALID_DISPOSITION:
			return "EXCEPTION_INVALID_DISPOSITION";
		case EXCEPTION_GUARD_PAGE:
			return "EXCEPTION_GUARD_PAGE";
		case EXCEPTION_INVALID_HANDLE:
			return "EXCEPTION_INVALID_HANDLE";
		default:
			return "unknown exception";
		}
	}

private:
	unsigned int code;
};


//
// AssertionFailedError クラス
//
///////////////////////////////////////////////////////////////////////////////

class AssertionFailedError : public Throwable
{
public:
	AssertionFailedError(DWORD ebp, DWORD esp, DWORD eip, const char *msg) : Throwable(ebp, esp, eip), message(msg)
	{
	}

	void printStackTrace(FILE *stream)
	{
		// アサートに失敗した場合は Assert クラスのメソッドの中でレジスタ値を調べているため
		// スタックトレースの一番最初は assertXxxx() の内部を指している
		// そのままではなく一番最初をスキップして二番目から出力するようにする

		if (nextStackElement() && nextStackElement()) {

			_ftprintf(stream, "%s(%u): AssertionFailedError: %s\n", getFileName(), getLineNumber(), getMessage());

			do {
				Throwable::printStackTrace(stream);

			} while (nextStackElement());
		}
	}

	const char *getMessage()
	{
		return message;
	}

private:
	const char *message;
};


//
// AssertionFailedErrorT テンプレート
//
///////////////////////////////////////////////////////////////////////////////

template <class A> class AssertionFailedErrorT : public AssertionFailedError
{
public:
	AssertionFailedErrorT(DWORD ebp, DWORD esp, DWORD eip, const char *format, A exp, A act) : AssertionFailedError(ebp, esp, eip, format), expected(exp), actual(act)
	{
	}

	void printStackTrace(FILE *stream)
	{
		// アサートに失敗した場合は Assert クラスのメソッドの中でレジスタ値を調べているため
		// スタックトレースの一番最初は assertXxxx() の内部を指している
		// そのままではなく一番最初をスキップして二番目から出力するようにする

		if (nextStackElement() && nextStackElement()) {

			_ftprintf(stream, "%s(%u): AssertionFailedError: ", getFileName(), getLineNumber());
			_ftprintf(stream, getMessage(), expected, actual);
			_fputtc('\n', stream);

			do {
				Throwable::printStackTrace(stream);

			} while (nextStackElement());
		}
	}

private:
	A expected;
	A actual;
};


//
// UserDefinedException クラス
//
///////////////////////////////////////////////////////////////////////////////

class UserDefinedException : public Throwable
{
public:
	UserDefinedException(DWORD ebp, DWORD esp, DWORD eip, const char *msg) : Throwable(ebp, esp, eip), message(msg)
	{
	}

	void printStackTrace(FILE *stream)
	{
		// 例外の発生個所をタグジャンプ可能なフォーマットで出力

		if (nextStackElement()) {

			_ftprintf(stream, "%s(%u): UserDefinedException: %s\n", getFileName(), getLineNumber(), getMessage());

			Throwable::printStackTrace(stream);
		}
	}

	const char *getMessage()
	{
		return message;
	}

private:
	const char *message;
};


//
// レジスタ値を取得するための関数
//
///////////////////////////////////////////////////////////////////////////////

DWORD __declspec(naked) GetEbp()
{
	__asm mov eax, ebp;
    __asm ret;
}

DWORD __declspec(naked) GetEsp()
{
	__asm mov eax, esp;
    __asm ret;
}

DWORD __declspec(naked) GetEip()
{
    __asm mov eax, [esp];
    __asm ret;
}


//
// Assert クラス
//
///////////////////////////////////////////////////////////////////////////////

void Assert::fail()
{
	throw AssertionFailedError(GetEbp(), GetEsp(), GetEip(), "");
}

void Assert::assertTrue(bool condition)
{
	if (!condition) {
		throw AssertionFailedError(GetEbp(), GetEsp(), GetEip(), "");
	}
}

void Assert::assertFalse(bool condition)
{
	if (condition) {
		throw AssertionFailedError(GetEbp(), GetEsp(), GetEip(), "");
	}
}

void Assert::assertNull(void *object)
{
	if (object) {
		throw AssertionFailedError(GetEbp(), GetEsp(), GetEip(), "");
	}
}

void Assert::assertNotNull(void *object)
{
	if (!object) {
		throw AssertionFailedError(GetEbp(), GetEsp(), GetEip(), "");
	}
}

void Assert::assertEquals(bool expected, bool actual)
{
	if (expected != actual) {
		throw AssertionFailedErrorT<const char*>(GetEbp(), GetEsp(), GetEip(), "expected:<%s> but was:<%s>", expected ? "true" : "false", actual ? "true" : "false");
	}
}

void Assert::assertEquals(const void *expected, const void *actual)
{
	if (expected != actual) {
		throw AssertionFailedErrorT<const void *>(GetEbp(), GetEsp(), GetEip(), "expected:<0x%08X> but was:<0x%08X>", expected, actual);
	}
}

void Assert::assertEquals(const char *expected, const char *actual)
{
	if (strcmp(expected, actual) != 0) {
		throw AssertionFailedErrorT<const char *>(GetEbp(), GetEsp(), GetEip(), "expected:<%s> but was:<%s>", expected, actual);
	}
}

void Assert::assertEquals(signed int expected, signed int actual)
{
	if (expected != actual) {
		throw AssertionFailedErrorT<signed int>(GetEbp(), GetEsp(), GetEip(), "expected:<%d> but was:<%d>", expected, actual);
	}
}

void Assert::assertEquals(signed long expected, signed long actual)
{
	if (expected != actual) {
		throw AssertionFailedErrorT<signed long>(GetEbp(), GetEsp(), GetEip(), "expected:<%d> but was:<%d>", expected, actual);
	}
}

void Assert::assertEquals(unsigned int expected, unsigned int actual)
{
	if (expected != actual) {
		throw AssertionFailedErrorT<unsigned int>(GetEbp(), GetEsp(), GetEip(), "expected:<%u> but was:<%u>", expected, actual);
	}
}

void Assert::assertEquals(unsigned long expected, unsigned long actual)
{
	if (expected != actual) {
		throw AssertionFailedErrorT<unsigned long>(GetEbp(), GetEsp(), GetEip(), "expected:<%u> but was:<%u>", expected, actual);
	}
}


//
// 構造化例外の変換処理
//
///////////////////////////////////////////////////////////////////////////////

void translateStructuredException(unsigned int code, EXCEPTION_POINTERS *pointers)
{
    throw StructuredException(pointers, code);
}


//
// 共用体を使って通常は許されないキャストを行う
//
///////////////////////////////////////////////////////////////////////////////

template <typename type1, typename type2> type1 hyper_cast(type2 cast)
{
	union { type1 result; type2 cast; } u;

	u.cast = cast;

	return u.result;
}


//
// テストの実行時間を高精度で計測する
//
///////////////////////////////////////////////////////////////////////////////

class Counter
{
public:
	Counter() : frequency(0), before(0)
	{
		::QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);

		start();
	}

	void start()
	{
		::QueryPerformanceCounter((LARGE_INTEGER *)&before);
	}

	unsigned int stop()
	{
		__int64 now;

		::QueryPerformanceCounter((LARGE_INTEGER *)&now);

		if (frequency) {

			return (unsigned int)((now - before) * 1000 / frequency);
		}

		return 0;
	}

private:
	__int64 frequency;
	__int64 before;
};


//
// テストメソッドの実行
//
///////////////////////////////////////////////////////////////////////////////

class TestCaseContext
{
public:
	TestCaseContext(TestResult &result, BaseTestCase &base, const char *name);
	static BOOL CALLBACK EnumSymbolCallback(LPSTR SymbolName, ULONG SymbolAddress, ULONG SymbolSize, PVOID UserContext);
private:
	TestResult &testResult;
	BaseTestCase &testCase;
	const char *className;
};

TestCaseContext::TestCaseContext(TestResult &result, BaseTestCase &base, const char *name) : testResult(result), testCase(base), className(name)
{
	// 修飾なしのクラス名のみを扱う

	const char *head = "class ";

	if (_tcsstr(className, head) == className) {

		className += strlen(head);
	}
}

BOOL CALLBACK TestCaseContext::EnumSymbolCallback(LPSTR SymbolName, ULONG SymbolAddress, ULONG SymbolSize, PVOID UserContext)
{
	// シンボル名をコピー後、クラス名とメソッド名に切り分ける

	TCHAR symbolName[MAX_SYMBOLNAME] = { 0 };

	_tcsncpy(symbolName, SymbolName, MAX_SYMBOLNAME - 1);

	if (TCHAR *methodName = _tcsstr(symbolName, "::")) {

		*methodName++ = '\0';
		*methodName++ = '\0';

		// シンボルがテストケースのもので、かつテストメソッドであることをチェック

		TestCaseContext *Context = (TestCaseContext *) UserContext;

		if ((_tcscmp(symbolName, Context->className) == 0) && (_tcsncmp(methodName, "test", 4) == 0)) {

			// テストを実行

			void (Test::*testMethod)() = hyper_cast<void (Test::*)()>(SymbolAddress);

			Context->testCase.run(Context->testResult, testMethod);
		}
	}

	return TRUE;
}


//
// Test インタフェース
//
///////////////////////////////////////////////////////////////////////////////

Test::Test() : tests(NULL)
{
}

Test::~Test()
{
}

Test *Test::addTest(Test *test)
{
	if (tests) {

		tests->addTest(test);

	} else {

		tests = test;
	}

	return test;
}

Test *Test::nextTest()
{
	return tests;
}


//
// BaseTestCase クラス
//
///////////////////////////////////////////////////////////////////////////////

BaseTestCase::BaseTestCase()
{
}

BaseTestCase::~BaseTestCase()
{
}

void BaseTestCase::setUp()
{
	// デフォルトでは何もしない
}

void BaseTestCase::tearDown()
{
	// デフォルトでは何もしない
}

void BaseTestCase::run(TestResult &result)
{
	// シンボル名を列挙しこのクラスのテストメソッドだけを実行

	TestCaseContext Context(result, *this, getClass().name());

	::SymEnumerateSymbols(::GetCurrentProcess(), (DWORD)::GetModuleHandle(NULL), TestCaseContext::EnumSymbolCallback, &Context);
}

void BaseTestCase::run(TestResult &result, void (Test::*testMethod)())
{
	// 構造化例外の変換関数を設定

	void *se_trans_func = _set_se_translator(translateStructuredException);

	// テストメソッドの呼び出し

	result.startTest(*this);

	Counter counter;

	try {
		setUp();

		(this->*testMethod)();
	}
	catch (AssertionFailedError &e) {

		result.addFailure(*this, counter.stop(), e);
	}
	catch (StructuredException &e) {

		result.addError(*this, counter.stop(), e);
	}
	catch (...) {

		result.addError(*this, counter.stop(), UserDefinedException(GetEbp(), GetEsp(), hyper_cast<DWORD>(testMethod), ""));
	}

	try {
		tearDown();
	}
	catch (AssertionFailedError &e) {

		result.addFailure(*this, counter.stop(), e);
	}
	catch (StructuredException &e) {

		result.addError(*this, counter.stop(), e);
	}
	catch (...) {

		result.addError(*this, counter.stop(), UserDefinedException(GetEbp(), GetEsp(), GetEip(), ""));
	}

	result.endTest(*this, counter.stop());

	// 構造化例外の変換関数を元に戻す

	_set_se_translator((_se_translator_function)se_trans_func);
}


//
// TestSuite クラス
//
///////////////////////////////////////////////////////////////////////////////

TestSuite::TestSuite() : tests(NULL)
{
}

TestSuite::~TestSuite()
{
	tests = NULL;
}

void TestSuite::run(TestResult &result)
{
	// 保持しているテストケースをすべて実行

	Test *test = tests;

	while (test) {

		test->run(result);

		test = test->nextTest();
	}
}

void TestSuite::addTestCase(BaseTestCase &testCase)
{
	// テストスーツにテストケースを追加する
	// テストケースは連結可能になっているため管理用の特別なメモリは必要ない

	if (tests) {

		tests->addTest(&testCase);

	} else {

		tests = &testCase;
	}
}


//
// TestRunner クラス
//
///////////////////////////////////////////////////////////////////////////////

TestRunner::TestRunner(FILE *fp) : stream(fp), runtime(0), runs(0), errors(0), failures(0)
{
}

TestRunner::~TestRunner()
{
}

int TestRunner::runCount()
{
	return runs;
}

int TestRunner::errorCount()
{
	return errors;
}

int TestRunner::failureCount()
{
	return failures;
}

int TestRunner::run(Test &test)
{
	test.run(*this);

	_ftprintf(stream, "\nTests run: %u, Failures: %u, Errors: %u, Time elapsed: %u.%03u sec\n", runCount(), failureCount(), errorCount(), runtime / 1000, runtime % 1000);

	return failureCount() + errorCount();
}

void TestRunner::startTest(Test &test)
{
}

void TestRunner::endTest(Test &test, unsigned int time)
{
	runs++;

	runtime += time;
}

void TestRunner::addError(Test &test, unsigned int time, Throwable &throwable)
{
	errors++;

	runtime += time;

	throwable.printStackTrace(stream);
}

void TestRunner::addFailure(Test &test, unsigned int time, Throwable &throwable)
{
	failures++;

	runtime += time;

	throwable.printStackTrace(stream);
}
