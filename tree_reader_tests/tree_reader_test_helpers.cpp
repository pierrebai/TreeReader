#include "tree_reader_test_helpers.h"

namespace dak::tree_reader_tests
{
	using namespace std;
	using namespace dak::tree_reader;

	shared_ptr<text_lines_text_holder> Createtext_lines()
	{
		auto textlines = make_shared<text_lines_text_holder>();

		textlines->lines.push_back(L"abc");
		textlines->lines.push_back(L"def");
		textlines->lines.push_back(L"ghi");
		textlines->lines.push_back(L"jkl");
		textlines->lines.push_back(L"mno");
		textlines->lines.push_back(L"pqr");
		textlines->lines.push_back(L"stu");
		textlines->lines.push_back(L"vwx");

		return textlines;
	}

	text_tree create_simple_tree()
	{
		text_tree textTree;

       auto textlines = Createtext_lines();
       textTree.source_text_lines = textlines;

		 auto r0 = textTree.add_child(nullptr, textlines->lines.at(0).c_str());
		 auto r0c0 = textTree.add_child(r0, textlines->lines.at(1).c_str());
		 auto r0c1 = textTree.add_child(r0, textlines->lines.at(2).c_str());
		 auto r0c0c0 = textTree.add_child(r0c0, textlines->lines.at(3).c_str());
		 auto r0c1c0 = textTree.add_child(r0c1, textlines->lines.at(4).c_str());
		 auto r0c1c0c0 = textTree.add_child(r0c1c0, textlines->lines.at(5).c_str());
		 auto r0c1c0c1 = textTree.add_child(r0c1c0, textlines->lines.at(6).c_str());
		 auto r0c1c0c1c0 = textTree.add_child(r0c1c0c1, textlines->lines.at(7).c_str());

		 return textTree;
	}
}
