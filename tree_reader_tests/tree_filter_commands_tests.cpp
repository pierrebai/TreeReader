#include "GlobalCommands.h"

#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace TreeReader;
using namespace std;

namespace TreeReaderTests
{		
	TEST_CLASS(TreeFilterCommandsTests)
	{
	public:
		
		TEST_METHOD(SaveLoadOptions)
		{
         GlobalCommands ctx;

         ctx.Options.OutputLineIndent = L"abc";
         ctx.Options.ReadOptions.InputFilter = L"def";
         ctx.Options.ReadOptions.InputIndent = L"ghi";
         ctx.Options.ReadOptions.TabSize = 5;

         wostringstream ostream;
         ctx.SaveOptions(ostream);

         wistringstream istream(ostream.str());
         GlobalCommands ctx2;
         ctx2.LoadOptions(istream);

         Assert::AreEqual(L"abc", ctx2.Options.OutputLineIndent.c_str());
         Assert::AreEqual(L"def", ctx2.Options.ReadOptions.InputFilter.c_str());
         Assert::AreEqual(L"ghi", ctx2.Options.ReadOptions.InputIndent.c_str());
         Assert::AreEqual<size_t>(5, ctx2.Options.ReadOptions.TabSize);
      }
	};
}
