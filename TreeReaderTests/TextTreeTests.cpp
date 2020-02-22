#include "TextTree.h"
#include "TreeReaderTestHelpers.h"
#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace TreeReaderTests
{
	TEST_CLASS(TextTreeTests)
	{
	public:

		TEST_METHOD(PrintEmptyTree)
		{
			wostringstream sstream;
			sstream << TextTree();

			const wchar_t expectedOutput[] = L"";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(CountSimpleTree)
		{
			const TextTree tree = CreateSimpleTree();

			Assert::AreEqual<size_t>(2, tree.CountChildren(0));
			Assert::AreEqual<size_t>(1, tree.CountChildren(1));
			Assert::AreEqual<size_t>(1, tree.CountChildren(2));
			Assert::AreEqual<size_t>(0, tree.CountChildren(3));
			Assert::AreEqual<size_t>(2, tree.CountChildren(4));
			Assert::AreEqual<size_t>(0, tree.CountChildren(5));
			Assert::AreEqual<size_t>(1, tree.CountChildren(6));
			Assert::AreEqual<size_t>(0, tree.CountChildren(7));

			Assert::AreEqual<size_t>(1, tree.CountSiblings(0));
			Assert::AreEqual<size_t>(2, tree.CountSiblings(1));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(2));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(3));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(4));
			Assert::AreEqual<size_t>(2, tree.CountSiblings(5));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(6));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(7));
		}

		TEST_METHOD(PrintSimpleTree)
		{
			wostringstream sstream;
			sstream << CreateSimpleTree();

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  def\n"
				L"    jkl\n"
				L"  ghi\n"
				L"    mno\n"
				L"      pqr\n"
				L"      stu\n"
				L"        vwx\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithDotDotIndent)
		{
			wostringstream sstream;
			PrintTree(sstream, CreateSimpleTree(), L"..");

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"..def\n"
				L"....jkl\n"
				L"..ghi\n"
				L"....mno\n"
				L"......pqr\n"
				L"......stu\n"
				L"........vwx\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}
	};
}
