#include "dak/tree_reader/text_tree.h"
#include "tree_reader_test_helpers.h"

#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace dak::tree_reader;

namespace dak::tree_reader_tests
{
	TEST_CLASS(text_tree_tests)
	{
	public:

		TEST_METHOD(print_empty_tree)
		{
			wostringstream sstream;
			sstream << text_tree();

			const wchar_t expectedOutput[] = L"";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(count_simple_tree)
		{
			const text_tree tree = create_simple_tree();

         Assert::AreEqual<size_t>(1, tree.roots.size());

			Assert::AreEqual<size_t>(2, tree.count_children(tree.roots[0]));
			Assert::AreEqual<size_t>(1, tree.count_children(tree.roots[0]->children[0]));
         Assert::AreEqual<size_t>(0, tree.count_children(tree.roots[0]->children[0]->children[0]));
         Assert::AreEqual<size_t>(1, tree.count_children(tree.roots[0]->children[1]));
			Assert::AreEqual<size_t>(2, tree.count_children(tree.roots[0]->children[1]->children[0]));
			Assert::AreEqual<size_t>(0, tree.count_children(tree.roots[0]->children[1]->children[0]->children[0]));
			Assert::AreEqual<size_t>(1, tree.count_children(tree.roots[0]->children[1]->children[0]->children[1]));
			Assert::AreEqual<size_t>(0, tree.count_children(tree.roots[0]->children[1]->children[0]->children[1]->children[0]));

			Assert::AreEqual<size_t>(1, tree.count_siblings(tree.roots[0]));
         Assert::AreEqual<size_t>(2, tree.count_siblings(tree.roots[0]->children[0]));
         Assert::AreEqual<size_t>(1, tree.count_siblings(tree.roots[0]->children[0]->children[0]));
         Assert::AreEqual<size_t>(2, tree.count_siblings(tree.roots[0]->children[1]));
         Assert::AreEqual<size_t>(1, tree.count_siblings(tree.roots[0]->children[1]->children[0]));
         Assert::AreEqual<size_t>(2, tree.count_siblings(tree.roots[0]->children[1]->children[0]->children[0]));
         Assert::AreEqual<size_t>(2, tree.count_siblings(tree.roots[0]->children[1]->children[0]->children[1]));
         Assert::AreEqual<size_t>(1, tree.count_siblings(tree.roots[0]->children[1]->children[0]->children[1]->children[0]));

         Assert::AreEqual<size_t>(0, tree.count_ancestors(tree.roots[0]));
         Assert::AreEqual<size_t>(1, tree.count_ancestors(tree.roots[0]->children[0]));
         Assert::AreEqual<size_t>(2, tree.count_ancestors(tree.roots[0]->children[0]->children[0]));
         Assert::AreEqual<size_t>(1, tree.count_ancestors(tree.roots[0]->children[1]));
         Assert::AreEqual<size_t>(2, tree.count_ancestors(tree.roots[0]->children[1]->children[0]));
         Assert::AreEqual<size_t>(3, tree.count_ancestors(tree.roots[0]->children[1]->children[0]->children[0]));
         Assert::AreEqual<size_t>(3, tree.count_ancestors(tree.roots[0]->children[1]->children[0]->children[1]));
         Assert::AreEqual<size_t>(4, tree.count_ancestors(tree.roots[0]->children[1]->children[0]->children[1]->children[0]));
      }

		TEST_METHOD(print_simple_tree)
		{
			wostringstream sstream;
			sstream << create_simple_tree();

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

		TEST_METHOD(print_simple_tree_with_dot_dot_indent)
		{
			wostringstream sstream;
			print_tree(sstream, create_simple_tree(), L"..");

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
