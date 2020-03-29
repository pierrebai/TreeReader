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

		 auto r0 = textTree.AddChild(nullptr, textLines->Lines.at(0).c_str());
		 auto r0c0 = textTree.AddChild(r0, textLines->Lines.at(1).c_str());
		 auto r0c1 = textTree.AddChild(r0, textLines->Lines.at(2).c_str());
		 auto r0c0c0 = textTree.AddChild(r0c0, textLines->Lines.at(3).c_str());
		 auto r0c1c0 = textTree.AddChild(r0c1, textLines->Lines.at(4).c_str());
		 auto r0c1c0c0 = textTree.AddChild(r0c1c0, textLines->Lines.at(5).c_str());
		 auto r0c1c0c1 = textTree.AddChild(r0c1c0, textLines->Lines.at(6).c_str());
		 auto r0c1c0c1c0 = textTree.AddChild(r0c1c0c1, textLines->Lines.at(7).c_str());

		 return textTree;
	}
}
