#include "TreeReader.h"
#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TreeReaderTests
{
	TEST_CLASS(TreeReaderTests)
	{
	public:
		
		TEST_METHOD(PrintEmptyTree)
		{
			std::wostringstream sstream;

			sstream << TextTree();

			const wchar_t expectedOutput[] = L"";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTree)
		{
			std::wostringstream sstream;

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

		TEST_METHOD(PrintFilteredSimpleTree)
		{
			std::wostringstream sstream;

			TextTree filtered;
			ContainsTextFilter filter(L"g");

			FilterTree(CreateSimpleTree(), filtered, filter);
			sstream << filtered;

			const wchar_t expectedOutput[] = L"ghi\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

	private:
			TextTree CreateSimpleTree()
			{
				TextTree textTree;

				auto textLines = std::make_shared<TextLines>();
				textLines->push_back(L"abc");
				textLines->push_back(L"def");
				textLines->push_back(L"ghi");
				textLines->push_back(L"jkl");
				textLines->push_back(L"mno");
				textLines->push_back(L"pqr");
				textLines->push_back(L"stu");
				textLines->push_back(L"vwx");
				textTree.SourceTextLines = textLines;

				textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(0), 1, 2 });
				textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(1), 3, 1 });
				textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(2), 4, 1 });
				textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(3), 0, 0 });
				textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(4), 5, 2 });
				textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(5), 0, 0 });
				textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(6), 7, 1 });
				textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(7), 0, 0 });

				return textTree;
			}
	};
}
