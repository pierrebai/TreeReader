#include "TreeReaderHelpers.h"
#include "CppUnitTest.h"

#include <iomanip>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
	template<> inline std::wstring ToString<vector<wstring>>(const vector<wstring>& v)
	{
		wstringstream stream;
		for (const auto& text : v)
			stream << quoted(text) << L" ";
		return stream.str();
	}
}

namespace TreeReaderTests
{
	TEST_CLASS(TreeReaderHelpersTests)
	{
	public:

		TEST_METHOD(SimpleSplit)
		{
			Assert::AreEqual(vector<wstring>{ L"a", L"b", L"c" }, Split(L"a b c"));
			Assert::AreEqual(vector<wstring>{ L"aaa", L"bbb", L"ccc" }, Split(L"aaa bbb ccc"));
			Assert::AreEqual(vector<wstring>{ L"a", L"b", L"c" }, Split(L"a    b  c"));
			Assert::AreEqual(vector<wstring>{ L"a", L"", L"", L"", L"b", L"", L"c" }, Split(L"a    b  c", L' ', SplitOptions::KeepEmpty));
			Assert::AreEqual(vector<wstring>{ L"a", L"b", L"c" }, Split(L"a,b,c", ','));
			Assert::AreEqual(vector<wstring>{ L"a", L"b", L"c" }, Split(L"a,b,,c", ','));
			Assert::AreEqual(vector<wstring>{ L"a", L"b", L"", L"c" }, Split(L"a,b,,c", ',', SplitOptions::KeepEmpty));
		}

		TEST_METHOD(SimpleJoin)
		{
			Assert::AreEqual(wstring(L"a b c"), Join(vector<wstring>{ L"a", L"b", L"c" }));
			Assert::AreEqual(wstring(L"aaa bbb ccc"), Join(vector<wstring>{ L"aaa", L"bbb", L"ccc" }));
			Assert::AreEqual(wstring(L"aaa,bbb,ccc"), Join(vector<wstring>{ L"aaa", L"bbb", L"ccc" }, L','));
		}
	};
}
