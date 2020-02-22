#include "SimpleTreeReader.h"
#include "TreeReaderTestHelpers.h"
#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace TreeReaderTests
{
	TEST_CLASS(SimpleTreeReaderTests)
	{
	public:

		TEST_METHOD(ReadSimpleTree)
		{
			wstringstream sstream;
			sstream << CreateSimpleTree();

			sstream.flush();
			sstream.seekg(0);

			TextTree tree = ReadSimpleTextTree(sstream);

			wostringstream sstream2;
			sstream2 << tree;

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  def\n"
				L"    jkl\n"
				L"  ghi\n"
				L"    mno\n"
				L"      pqr\n"
				L"      stu\n"
				L"        vwx\n";
			Assert::AreEqual(expectedOutput, sstream2.str().c_str());
		}

		TEST_METHOD(ReadSimpleTreeWithInputFilter)
		{
			wstringstream sstream;
			sstream << CreateSimpleTree();

			sstream.flush();
			sstream.seekg(0);

			ReadSimpleTextTreeOptions options;
			options.InputFilter = L"([^bek]*)";

			TextTree tree = ReadSimpleTextTree(sstream, options);

			wostringstream sstream2;
			sstream2 << tree;

			const wchar_t expectedOutput[] =
				L"ac\n"
				L"  df\n"
				L"    jl\n"
				L"  ghi\n"
				L"    mno\n"
				L"      pqr\n"
				L"      stu\n"
				L"        vwx\n";
			Assert::AreEqual(expectedOutput, sstream2.str().c_str());
		}
	};
}
