#include "dak/tree_reader/tree_reader.h"
#include "tree_reader_test_helpers.h"

#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace dak::tree_reader;

namespace dak::tree_reader_tests
{
	TEST_CLASS(tree_filter_maker_tests)
	{
	public:

      TEST_METHOD(ConvertToTextacceptFilter)
      {
         auto filter = accept();

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \naccept [ ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<accept_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);
      }

      TEST_METHOD(ConvertToTextstopFilter)
      {
         auto filter = stop();

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nstop [ ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<stop_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);
      }

      TEST_METHOD(ConvertToTextstopWhenKeptFilter)
      {
         auto filter = stop_when_kept(accept());

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nstop-when-kept [ \n accept [ ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<stop_when_kept_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::IsTrue(dynamic_pointer_cast<accept_tree_filter>(rebuilt->filter) != nullptr);
      }

      TEST_METHOD(ConvertToTextcontainsFilter)
      {
         auto filter = contains(L"\"abc\"");

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \ncontains [ \"\\\"abc\\\"\" ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<contains_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::AreEqual(L"\"abc\"", rebuilt->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextuniqueFilter)
      {
         auto filter = unique();

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nunique [ ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<unique_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);
      }

      TEST_METHOD(ConvertToTextregexFilter)
      {
         auto filter = dak::tree_reader::regex(L"[abc]*");

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nregex [ \"[abc]*\" ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<regex_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::AreEqual(L"[abc]*", rebuilt->regexTextForm.c_str());
      }

      TEST_METHOD(ConvertToTextnotacceptFilter)
      {
         auto filter = not(accept());

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nnot [ \n accept [ ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<not_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::IsTrue(dynamic_pointer_cast<accept_tree_filter>(rebuilt->filter) != nullptr);
      }

      TEST_METHOD(ConvertToTextOrFilter)
      {
         auto filter = or(contains(L"a"), contains(L"b"));

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nor [ \n contains [ \"a\" ], \n contains [ \"b\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<or_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual<size_t>(2, rebuilt->named_filters.size());

         auto lhs = dynamic_pointer_cast<contains_tree_filter>(rebuilt->named_filters[0]);
         Assert::IsTrue(lhs != nullptr);
         Assert::AreEqual(L"a", lhs->Contained.c_str());

         auto rhs = dynamic_pointer_cast<contains_tree_filter>(rebuilt->named_filters[1]);
         Assert::IsTrue(rhs != nullptr);
         Assert::AreEqual(L"b", rhs->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextAndFilter)
      {
         auto filter = and(contains(L"a"), unique());

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nand [ \n contains [ \"a\" ], \n unique [ ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<and_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual<size_t>(2, rebuilt->named_filters.size());

         auto lhs = dynamic_pointer_cast<contains_tree_filter>(rebuilt->named_filters[0]);
         Assert::IsTrue(lhs != nullptr);
         Assert::AreEqual(L"a", lhs->Contained.c_str());

         auto rhs = dynamic_pointer_cast<unique_tree_filter>(rebuilt->named_filters[1]);
         Assert::IsTrue(rhs != nullptr);
      }

      TEST_METHOD(ConvertToTextUnderFilter)
      {
         auto filter = under(contains(L"a"), true);

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nunder [ true, \n contains [ \"a\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<under_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::IsTrue(rebuilt->include_self);

         auto under = dynamic_pointer_cast<contains_tree_filter>(rebuilt->filter);
         Assert::IsTrue(under != nullptr);
         Assert::AreEqual(L"a", under->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextremovechildrenFilter)
      {
         auto filter = no_child(contains(L"abc"));

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nno-child [ false, \n contains [ \"abc\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<remove_children_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::IsFalse(rebuilt->include_self);

         auto subFilter = dynamic_pointer_cast<contains_tree_filter>(rebuilt->filter);
         Assert::IsTrue(subFilter != nullptr);
         Assert::AreEqual(L"abc", subFilter->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextRangeFilter)
      {
         auto filter = level_range(7, 9);

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nrange [ 7, 9 ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<level_range_tree_filter>(convert_text_to_filter(text, named_filters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual<size_t>(7, rebuilt->min_level);
         Assert::AreEqual<size_t>(9, rebuilt->max_level);
      }

      TEST_METHOD(ConvertToTextNamedFilter)
      {
         named_filters known;
         known.add(L"abc", contains(L"abc"));

         auto filter = known.get(L"abc");

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nnamed [ \"abc\" ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<named_tree_filter>(convert_text_to_filter(text, known));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual(wstring(L"abc"), rebuilt->Name);
         Assert::IsTrue(known.get(L"abc")->filter == rebuilt->filter);
      }

      TEST_METHOD(ConvertSimpleText)
      {
         auto filter = convert_simple_text_to_filter(L"( a & b & ! c ) | ( > d | ( <= 3 & * & . ) ) ", named_filters());

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nor [ \n and [ \n  contains [ \"a\" ], \n  contains [ \"b\" ], \n  not [ \n   contains [ \"c\" ] ] ], \n or [ \n  under [ true, \n   contains [ \"d\" ] ], \n  and [ \n   range [ 0, 3 ], \n   accept [ ], \n   stop [ ] ] ] ]", text.c_str());
      }

      TEST_METHOD(ConvertSimpleTextWithIfSibling)
      {
         auto filter = convert_simple_text_to_filter(L"a ?= b", named_filters());

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nand [ \n contains [ \"a\" ], \n if-sib [ \n  contains [ \"b\" ] ] ]", text.c_str());
      }

      TEST_METHOD(ConvertSimpleTextWithIfSubTree)
      {
         auto filter = convert_simple_text_to_filter(L"a ?> b", named_filters());

         const wstring text = convert_filter_to_text(filter);

         Assert::AreEqual(L"V1: \nand [ \n contains [ \"a\" ], \n if-sub [ \n  contains [ \"b\" ] ] ]", text.c_str());
      }
   };
}
