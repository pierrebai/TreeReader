#include "TreeReader.h"
#include "TreeReaderTestHelpers.h"
#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace TreeReaderTests
{
	TEST_CLASS(TreeFilterMakerTests)
	{
	public:

      TEST_METHOD(ConvertToTextAcceptFilter)
      {
         auto filter = Accept();

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \naccept [ ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<AcceptTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);
      }

      TEST_METHOD(ConvertToTextStopFilter)
      {
         auto filter = Stop();

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nstop [ ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<StopTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);
      }

      TEST_METHOD(ConvertToTextContainsFilter)
      {
         auto filter = Contains(L"\"abc\"");

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \ncontains [ \"\\\"abc\\\"\" ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<ContainsTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::AreEqual(L"\"abc\"", rebuilt->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextRegexFilter)
      {
         auto filter = Regex(L"[abc]*");

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nregex [ \"[abc]*\" ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<RegexTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::AreEqual(L"[abc]*", rebuilt->RegexTextForm.c_str());
      }

      TEST_METHOD(ConvertToTextNotAcceptFilter)
      {
         auto filter = Not(Accept());

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nnot [ \n accept [ ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<NotTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::IsTrue(dynamic_pointer_cast<AcceptTreeFilter>(rebuilt->Filter) != nullptr);
      }

      TEST_METHOD(ConvertToTextOrFilter)
      {
         auto filter = Or(Contains(L"a"), Contains(L"b"));

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nor [ \n contains [ \"a\" ], \n contains [ \"b\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<OrTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual<size_t>(2, rebuilt->Filters.size());

         auto lhs = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Filters[0]);
         Assert::IsTrue(lhs != nullptr);
         Assert::AreEqual(L"a", lhs->Contained.c_str());

         auto rhs = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Filters[1]);
         Assert::IsTrue(rhs != nullptr);
         Assert::AreEqual(L"b", rhs->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextAndFilter)
      {
         auto filter = And(Contains(L"a"), Accept());

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nand [ \n contains [ \"a\" ], \n accept [ ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<AndTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual<size_t>(2, rebuilt->Filters.size());

         auto lhs = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Filters[0]);
         Assert::IsTrue(lhs != nullptr);
         Assert::AreEqual(L"a", lhs->Contained.c_str());

         auto rhs = dynamic_pointer_cast<AcceptTreeFilter>(rebuilt->Filters[1]);
         Assert::IsTrue(rhs != nullptr);
      }

      TEST_METHOD(ConvertToTextUnderFilter)
      {
         auto filter = Under(Contains(L"a"), true);

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nunder [ true, \n contains [ \"a\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<UnderTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::IsTrue(rebuilt->IncludeSelf);

         auto under = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Filter);
         Assert::IsTrue(under != nullptr);
         Assert::AreEqual(L"a", under->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextCountChildrenFilter)
      {
         auto filter = CountChildren(Contains(L"a"), 3, true);

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \ncount-sub [ true, 3, \n contains [ \"a\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<CountChildrenTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::AreEqual<size_t>(3, rebuilt->Count);

         Assert::IsTrue(rebuilt->IncludeSelf);

         auto under = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Filter);
         Assert::IsTrue(under != nullptr);
         Assert::AreEqual(L"a", under->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextCountSiblingsFilter)
      {
         auto filter = CountSiblings(Contains(L"abc"), 44, false);

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \ncount-sib [ false, 44, \n contains [ \"abc\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<CountSiblingsTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::AreEqual<size_t>(44, rebuilt->Count);

         Assert::IsFalse(rebuilt->IncludeSelf);

         auto under = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Filter);
         Assert::IsTrue(under != nullptr);
         Assert::AreEqual(L"abc", under->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextRemoveChildrenFilter)
      {
         auto filter = NoChild(Contains(L"abc"));

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nno-child [ false, \n contains [ \"abc\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<RemoveChildrenTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::IsFalse(rebuilt->RemoveSelf);

         auto subFilter = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Filter);
         Assert::IsTrue(subFilter != nullptr);
         Assert::AreEqual(L"abc", subFilter->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextRangeFilter)
      {
         auto filter = LevelRange(7, 9);

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nrange [ 7, 9 ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<LevelRangeTreeFilter>(ConvertTextToFilters(text, NamedFilters()));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual<size_t>(7, rebuilt->MinLevel);
         Assert::AreEqual<size_t>(9, rebuilt->MaxLevel);
      }

      TEST_METHOD(ConvertToTextNamedFilter)
      {
         NamedFilters known;
         known.Add(L"abc", Contains(L"abc"));

         auto filter = known.Get(L"abc");

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nnamed [ \"abc\" ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<NamedTreeFilter>(ConvertTextToFilters(text, known));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual(wstring(L"abc"), rebuilt->Name);
         Assert::IsTrue(known.Get(L"abc")->Filter == rebuilt->Filter);
      }

      TEST_METHOD(ConvertSimpleText)
      {
         auto filter = ConvertSimpleTextToFilters(L"( a & b & ! c ) | ( > d | ( <= 3 & * & . ) ) ", NamedFilters());

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nor [ \n and [ \n  contains [ \"a\" ], \n  contains [ \"b\" ], \n  not [ \n   contains [ \"c\" ] ] ], \n or [ \n  under [ true, \n   contains [ \"d\" ] ], \n  and [ \n   range [ 0, 3 ], \n   accept [ ], \n   stop [ ] ] ] ]", text.c_str());
      }

      TEST_METHOD(ConvertSimpleTextWithIfSibling)
      {
         auto filter = ConvertSimpleTextToFilters(L"a ?= b", NamedFilters());

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nand [ \n contains [ \"a\" ], \n if-sib [ \n  contains [ \"b\" ] ] ]", text.c_str());
      }

      TEST_METHOD(ConvertSimpleTextWithIfSubTree)
      {
         auto filter = ConvertSimpleTextToFilters(L"a ?> b", NamedFilters());

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nand [ \n contains [ \"a\" ], \n if-sub [ \n  contains [ \"b\" ] ] ]", text.c_str());
      }

      TEST_METHOD(ConvertSimpleTextWithCountChildren)
      {
         auto filter = ConvertSimpleTextToFilters(L"a #> 3 b", NamedFilters());

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nand [ \n contains [ \"a\" ], \n count-sub [ true, 3, \n  contains [ \"b\" ] ] ]", text.c_str());
      }

      TEST_METHOD(ConvertSimpleTextWithCountSiblings)
      {
         auto filter = ConvertSimpleTextToFilters(L"a #= 22 b", NamedFilters());

         const wstring text = ConvertFiltersToText(filter);

         Assert::AreEqual(L"V1: \nand [ \n contains [ \"a\" ], \n count-sib [ true, 22, \n  contains [ \"b\" ] ] ]", text.c_str());
      }
   };
}
