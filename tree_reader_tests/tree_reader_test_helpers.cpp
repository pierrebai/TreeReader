#include "tree_reader_test_helpers.h"

namespace dak::tree_reader_tests
{
	using namespace std;
	using namespace dak::tree_reader;

	shared_ptr<text_lines_text_holder_t> create_text_lines()
	{
		auto text_lines = make_shared<text_lines_text_holder_t>();

		text_lines->lines.push_back(L"abc");
		text_lines->lines.push_back(L"def");
		text_lines->lines.push_back(L"ghi");
		text_lines->lines.push_back(L"jkl");
		text_lines->lines.push_back(L"mno");
		text_lines->lines.push_back(L"pqr");
		text_lines->lines.push_back(L"stu");
		text_lines->lines.push_back(L"vwx");

		return text_lines;
	}

	text_tree_t create_simple_tree()
	{
		text_tree_t textTree;

       auto text_lines = create_text_lines();
       textTree.source_text_lines = text_lines;

		 auto r0 = textTree.add_child(nullptr, text_lines->lines.at(0).c_str());
		 auto r0c0 = textTree.add_child(r0, text_lines->lines.at(1).c_str());
		 auto r0c1 = textTree.add_child(r0, text_lines->lines.at(2).c_str());
		 auto r0c0c0 = textTree.add_child(r0c0, text_lines->lines.at(3).c_str());
		 auto r0c1c0 = textTree.add_child(r0c1, text_lines->lines.at(4).c_str());
		 auto r0c1c0c0 = textTree.add_child(r0c1c0, text_lines->lines.at(5).c_str());
		 auto r0c1c0c1 = textTree.add_child(r0c1c0, text_lines->lines.at(6).c_str());
		 auto r0c1c0c1c0 = textTree.add_child(r0c1c0c1, text_lines->lines.at(7).c_str());

		 return textTree;
	}
}
