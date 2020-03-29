#include "dak/tree_reader/global_commands.h"

#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::tree_reader;
using namespace std;

namespace dak::tree_reader_tests
{		
	TEST_CLASS(tree_filter_commands_tests)
	{
	public:
		
		TEST_METHOD(Saveload_options)
		{
         global_commands ctx;

         ctx.options.output_line_indent = L"abc";
         ctx.options.read_options.input_filter = L"def";
         ctx.options.read_options.input_indent = L"ghi";
         ctx.options.read_options.tab_size = 5;

         wostringstream ostream;
         ctx.save_options(ostream);

         wistringstream istream(ostream.str());
         global_commands ctx2;
         ctx2.load_options(istream);

         Assert::AreEqual(L"abc", ctx2.options.output_line_indent.c_str());
         Assert::AreEqual(L"def", ctx2.options.read_options.input_filter.c_str());
         Assert::AreEqual(L"ghi", ctx2.options.read_options.input_indent.c_str());
         Assert::AreEqual<size_t>(5, ctx2.options.read_options.tab_size);
      }
	};
}
