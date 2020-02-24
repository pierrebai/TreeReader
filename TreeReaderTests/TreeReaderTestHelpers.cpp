#include "TreeReaderTestHelpers.h"

namespace TreeReaderTests
{
	using namespace std;
	using namespace TreeReader;

	shared_ptr<TextLinesTextHolder> CreateTextLines()
	{
		auto textLines = make_shared<TextLinesTextHolder>();

		textLines->Lines.push_back(L"abc");
		textLines->Lines.push_back(L"def");
		textLines->Lines.push_back(L"ghi");
		textLines->Lines.push_back(L"jkl");
		textLines->Lines.push_back(L"mno");
		textLines->Lines.push_back(L"pqr");
		textLines->Lines.push_back(L"stu");
		textLines->Lines.push_back(L"vwx");

		return textLines;
	}

	TextTree CreateSimpleTree()
	{
		TextTree textTree;

       auto textLines = CreateTextLines();
       textTree.SourceTextLines = textLines;

       //                                                                           parent     sibling       child  count
		 textTree.Nodes.push_back(TextTree::Node{ textLines->Lines.at(0).c_str(), size_t(-1), size_t(-1),          1,     2 });
		 textTree.Nodes.push_back(TextTree::Node{ textLines->Lines.at(1).c_str(),          0,          2,          3,     1 });
       textTree.Nodes.push_back(TextTree::Node{ textLines->Lines.at(2).c_str(),          0, size_t(-1),          4,     1 });
       textTree.Nodes.push_back(TextTree::Node{ textLines->Lines.at(3).c_str(),          1, size_t(-1), size_t(-1),     0 });
       textTree.Nodes.push_back(TextTree::Node{ textLines->Lines.at(4).c_str(),          2, size_t(-1),          5,     2 });
       textTree.Nodes.push_back(TextTree::Node{ textLines->Lines.at(5).c_str(),          4,          6, size_t(-1),     0 });
       textTree.Nodes.push_back(TextTree::Node{ textLines->Lines.at(6).c_str(),          4, size_t(-1),          7,     1 });
       textTree.Nodes.push_back(TextTree::Node{ textLines->Lines.at(7).c_str(),          6, size_t(-1), size_t(-1),     0 });

       textTree.RootCount = 1;

		 return textTree;
	}
}
