#include "dak/tree_reader/tree_filter.h"
#include "dak/tree_reader/tree_filtering.h"
#include "tree_reader_test_helpers.h"

#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace dak::tree_reader;

namespace dak::tree_reader_tests
{
	TEST_CLASS(tree_filter_tests)
	{
	public:

		TEST_METHOD(PrintSimpleTreeWithcontainsFilter)
		{
			text_tree filtered;
			filter_tree(create_simple_tree(), filtered, *contains(L"g"));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] = L"ghi\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

      TEST_METHOD(PrintSimpleTreeWithuniqueFilter)
      {
         text_tree filtered;
         filter_tree(create_simple_tree(), filtered, *unique());

         wostringstream sstream;
         sstream << filtered;

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

      TEST_METHOD(PrintSimpleTreeWithexact_addressFilter)
      {
         text_tree tree = create_simple_tree();
         text_tree filtered;
         filter_tree(tree, filtered, *exact_address(tree.roots[0]->text_ptr));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] = L"abc\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithnotFilter)
		{
			text_tree filtered;
			filter_tree(create_simple_tree(), filtered, not(contains(L"f")));

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
         text_tree filtered;
         filter_tree(create_simple_tree(), filtered, if_subtree(contains(L"k")));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithIfSubTreeAndstopFilter)
      {
         text_tree filtered;
         filter_tree(create_simple_tree(), filtered, and(if_subtree(contains(L"f")), stop()));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithIfSiblingFilter)
      {
         text_tree filtered;
         filter_tree(create_simple_tree(), filtered, if_sibling(contains(L"t")));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"pqr\n"
            L"stu\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithOrFilter)
		{
			text_tree filtered;
			filter_tree(create_simple_tree(), filtered, or(contains(L"f"), contains(L"m")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"def\n"
				L"mno\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithremovechildrenFilter)
		{
			text_tree filtered;
			filter_tree(create_simple_tree(), filtered, no_child(contains(L"g")));

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
			text_tree filtered;
			filter_tree(create_simple_tree(), filtered, and(under(contains(L"g"), false), contains(L"s")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"stu\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithUnderAndSelf)
		{
			text_tree filtered;
			filter_tree(create_simple_tree(), filtered, under(contains(L"g"), true));

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

      TEST_METHOD(PrintSimpleTreeWithremovechildrenAndSelfFilter)
      {
         text_tree filtered;
         filter_tree(create_simple_tree(), filtered, no_child(contains(L"g"), true));

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
         text_tree filtered;
         filter_tree(create_simple_tree(), filtered, level_range(2, 3));

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
         text_tree filtered;
         filter_tree(create_simple_tree(), filtered, min_level(2));

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
         text_tree filtered;
         filter_tree(create_simple_tree(), filtered, max_level(1));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n"
            L"  ghi\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithnotregex)
		{
			text_tree filtered;
			filter_tree(create_simple_tree(), filtered, not(dak::tree_reader::regex(L"[g]")));

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
			text_tree filtered;
			filter_tree(create_simple_tree(), filtered, or(contains(L"d"), contains(L"s")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"def\n"
				L"stu\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

      TEST_METHOD(PrintSimpleTreeWithAsynccontainsFilter)
      {
         auto tree = make_shared<text_tree>(create_simple_tree());
         auto [fut, abort] = filter_tree_async(tree, contains(L"g"));

         wostringstream sstream;
         sstream << fut.get();

         const wchar_t expectedOutput[] = L"ghi\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

   };
}
