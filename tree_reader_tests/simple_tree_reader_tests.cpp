#include "dak/tree_reader/simple_tree_reader.h"
#include "tree_reader_test_helpers.h"

#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace dak::tree_reader;

namespace dak::tree_reader_tests
{
	TEST_CLASS(simple_tree_reader_tests)
	{
	public:

		TEST_METHOD(read_simple_tree)
		{
			wstringstream sstream;
			sstream << create_simple_tree();

			sstream.flush();
			sstream.seekg(0);

			text_tree_t tree = load_simple_text_tree(sstream);

			wostringstream sstream2;
			sstream2 << tree;

			const wchar_t expected_output[] =
				L"abc\n"
				L"  def\n"
				L"    jkl\n"
				L"  ghi\n"
				L"    mno\n"
				L"      pqr\n"
				L"      stu\n"
				L"        vwx\n";
			Assert::AreEqual(expected_output, sstream2.str().c_str());
		}

		TEST_METHOD(read_simple_tree_with_input_filter)
		{
			wstringstream sstream;
			sstream << create_simple_tree();

			sstream.flush();
			sstream.seekg(0);

			load_simple_text_tree_options_t options;
			options.input_filter = L"([^bek]*)";

			text_tree_t tree = load_simple_text_tree(sstream, options);

			wostringstream sstream2;
			sstream2 << tree;

			const wchar_t expected_output[] =
				L"ac\n"
				L"  df\n"
				L"    jl\n"
				L"  ghi\n"
				L"    mno\n"
				L"      pqr\n"
				L"      stu\n"
				L"        vwx\n";
			Assert::AreEqual(expected_output, sstream2.str().c_str());
		}
	};
}
