#include "TreeFilter.h"
#include "TreeReaderTestHelpers.h"
#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace TreeReaderTests
{
	TEST_CLASS(TreeFilterTests)
	{
	public:

		TEST_METHOD(PrintSimpleTreeWithContainsFilter)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Contains(L"g"));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] = L"ghi\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

      TEST_METHOD(PrintSimpleTreeWithExactAddressFilter)
      {
         TextTree tree = CreateSimpleTree();
         TextTree filtered;
         FilterTree(tree, filtered, ExactAddress(tree.Roots[0]->TextPtr));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] = L"abc\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithNotFilter)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Not(Contains(L"f")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  jkl\n"
				L"  ghi\n"
				L"    mno\n"
				L"      pqr\n"
				L"      stu\n"
				L"        vwx\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

      TEST_METHOD(PrintSimpleTreeWithIfSubTreeFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, IfSubTree(Contains(L"k")));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithIfSubTreeAndStopFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, And(IfSubTree(Contains(L"f")), Stop()));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithIfSiblingFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, IfSibling(Contains(L"t")));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"pqr\n"
            L"stu\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithOrFilter)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Or(Contains(L"f"), Contains(L"m")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"def\n"
				L"mno\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithRemoveChildrenFilter)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, NoChild(Contains(L"g")));

			wostringstream sstream;
			sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n"
            L"    jkl\n"
            L"  ghi\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithUnder)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, And(Under(Contains(L"g"), false), Contains(L"s")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"stu\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithUnderAndSelf)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Under(Contains(L"g"), true));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
            L"ghi\n"
            L"  mno\n"
            L"    pqr\n"
            L"    stu\n"
            L"      vwx\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

      TEST_METHOD(PrintSimpleTreeWithCountSiblings)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, CountSiblings(Contains(L"d"), 10, false));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"ghi\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithCountSiblingsIncludeSelf)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, CountSiblings(Contains(L"d"), 10, true));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"def\n"
            L"ghi\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithCountChildren)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, CountChildren(Contains(L"d"), 10, false));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"jkl\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithCountChildrenIncludeSelf)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, CountChildren(Contains(L"g"), 2, true));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"ghi\n"
            L"  mno\n"
            L"    pqr\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithRemoveChildrenAndSelfFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, NoChild(Contains(L"g"), true));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n"
            L"    jkl\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithLevelRangeFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, LevelRange(2, 3));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"jkl\n"
            L"mno\n"
            L"  pqr\n"
            L"  stu\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithMinLevelFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, MinLevel(2));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"jkl\n"
            L"mno\n"
            L"  pqr\n"
            L"  stu\n"
            L"    vwx\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithMaxLevelFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, MaxLevel(1));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n"
            L"  ghi\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithNotRegex)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Not(Regex(L"[g]")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  def\n"
				L"    jkl\n"
				L"  mno\n"
				L"    pqr\n"
				L"    stu\n"
				L"      vwx\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}


		TEST_METHOD(PrintSimpleTreeWithMultiTreeFilters)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Or(Contains(L"d"), Contains(L"s")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"def\n"
				L"stu\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

      TEST_METHOD(PrintSimpleTreeWithAsyncContainsFilter)
      {
         auto tree = make_shared<TextTree>(CreateSimpleTree());
         auto [fut, abort] = FilterTreeAsync(tree, Contains(L"g"));

         wostringstream sstream;
         sstream << fut.get();

         const wchar_t expectedOutput[] = L"ghi\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

   };
}
