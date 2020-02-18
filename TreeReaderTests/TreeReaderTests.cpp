#include "TreeReader.h"
#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace TreeReaderTests
{
	TEST_CLASS(TreeReaderTests)
	{
	public:

		TEST_METHOD(PrintEmptyTree)
		{
			wostringstream sstream;
			sstream << TextTree();

			const wchar_t expectedOutput[] = L"";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(CountSimpleTree)
		{
			const TextTree tree = CreateSimpleTree();

			Assert::AreEqual<size_t>(2, tree.CountChildren(0));
			Assert::AreEqual<size_t>(1, tree.CountChildren(1));
			Assert::AreEqual<size_t>(1, tree.CountChildren(2));
			Assert::AreEqual<size_t>(0, tree.CountChildren(3));
			Assert::AreEqual<size_t>(2, tree.CountChildren(4));
			Assert::AreEqual<size_t>(0, tree.CountChildren(5));
			Assert::AreEqual<size_t>(1, tree.CountChildren(6));
			Assert::AreEqual<size_t>(0, tree.CountChildren(7));

			Assert::AreEqual<size_t>(1, tree.CountSiblings(0));
			Assert::AreEqual<size_t>(2, tree.CountSiblings(1));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(2));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(3));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(4));
			Assert::AreEqual<size_t>(2, tree.CountSiblings(5));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(6));
			Assert::AreEqual<size_t>(1, tree.CountSiblings(7));
		}

		TEST_METHOD(PrintBuiltTree)
		{
			TextTree tree;

			tree.SourceTextLines = CreateTextLines();

			size_t r0 = tree.AddChild(0, &tree.SourceTextLines->at(0));
			size_t r0c0 = tree.AddChild(r0, &tree.SourceTextLines->at(1));
			size_t r0c1 = tree.AddChild(r0, &tree.SourceTextLines->at(2));
			size_t r0c2 = tree.AddSibling(r0c1, &tree.SourceTextLines->at(3));
			size_t r0c0c0 = tree.AddChild(r0c0, &tree.SourceTextLines->at(4));
			size_t r1 = tree.AddSibling(r0, &tree.SourceTextLines->at(5));

			wostringstream sstream;
			sstream << tree;

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  def\n"
				L"    mno\n"
				L"  ghi\n"
				L"  jkl\n"
				L"pqr\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTree)
		{
			wostringstream sstream;
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

		TEST_METHOD(PrintSimpleTreeWithContainsFilter)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Contains(L"g"));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] = L"ghi\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithNotFilter)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Not(Contains(L"f")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  jkl\n"
				L"  ghi\n"
				L"    mno\n"
				L"      pqr\n"
				L"      stu\n"
				L"        vwx\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithOrFilter)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Or(Contains(L"f"), Contains(L"m")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"def\n"
				L"mno\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithRemoveChildrenFilter)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, NoChild(Contains(L"g")));

			wostringstream sstream;
			sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n"
            L"    jkl\n"
            L"  ghi\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithUnder)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Under(Contains(L"g"), Contains(L"s"), false));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  def\n"
				L"    jkl\n"
				L"  ghi\n"
				L"    stu\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithUnderAndSelf)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Under(Contains(L"g"), Contains(L"s"), true));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  def\n"
				L"    jkl\n"
				L"  stu\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

		TEST_METHOD(PrintSimpleTreeWithRemoveChildrenAndSelfFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, NoChild(Contains(L"g"), true));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n"
            L"    jkl\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithLevelRangeFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, LevelRange(2, 3));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"jkl\n"
            L"mno\n"
            L"  pqr\n"
            L"  stu\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithMinLevelFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, MinLevel(2));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"jkl\n"
            L"mno\n"
            L"  pqr\n"
            L"  stu\n"
            L"    vwx\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithMaxLevelFilter)
      {
         TextTree filtered;
         FilterTree(CreateSimpleTree(), filtered, MaxLevel(1));

         wostringstream sstream;
         sstream << filtered;

         const wchar_t expectedOutput[] =
            L"abc\n"
            L"  def\n"
            L"  ghi\n";
         Assert::AreEqual(expectedOutput, sstream.str().c_str());
      }

      TEST_METHOD(PrintSimpleTreeWithNotRegex)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Not(Regex(L"[g]")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"abc\n"
				L"  def\n"
				L"    jkl\n"
				L"  mno\n"
				L"    pqr\n"
				L"    stu\n"
				L"      vwx\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}


		TEST_METHOD(PrintSimpleTreeWithMultiTreeFilters)
		{
			TextTree filtered;
			FilterTree(CreateSimpleTree(), filtered, Or(Contains(L"d"), Contains(L"s")));

			wostringstream sstream;
			sstream << filtered;

			const wchar_t expectedOutput[] =
				L"def\n"
				L"stu\n";
			Assert::AreEqual(expectedOutput, sstream.str().c_str());
		}

      TEST_METHOD(ConvertToTextAcceptFilter)
      {
         auto accept = All();

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: accept [ ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<AcceptTreeFilter>(ConvertTextToFilters(text));
         Assert::IsTrue(rebuilt != nullptr);
      }

      TEST_METHOD(ConvertToTextContainsFilter)
      {
         auto accept = Contains(L"\"abc\"");

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: contains [ \"\\\"abc\\\"\" ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<ContainsTreeFilter>(ConvertTextToFilters(text));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::AreEqual(L"\"abc\"", rebuilt->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextRegexFilter)
      {
         auto accept = Regex(L"[abc]*");

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: regex [ \"[abc]*\" ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<RegexTreeFilter>(ConvertTextToFilters(text));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::AreEqual(L"[abc]*", rebuilt->RegexTextForm.c_str());
      }

      TEST_METHOD(ConvertToTextNotAcceptFilter)
      {
         auto accept = Not(All());

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: not [ accept [ ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<NotTreeFilter>(ConvertTextToFilters(text));
         Assert::IsTrue(rebuilt != nullptr);
         Assert::IsTrue(dynamic_pointer_cast<AcceptTreeFilter>(rebuilt->Filter) != nullptr);
      }

      TEST_METHOD(ConvertToTextOrFilter)
      {
         auto accept = Or(Contains(L"a"), Contains(L"b"));

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: or [ contains [ \"a\" ], contains [ \"b\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<OrTreeFilter>(ConvertTextToFilters(text));
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
         auto accept = And(Contains(L"a"), All());

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: and [ contains [ \"a\" ], accept [ ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<AndTreeFilter>(ConvertTextToFilters(text));
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
         auto accept = Under(Contains(L"a"), All(), true);

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: under [ true, contains [ \"a\" ], accept [ ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<ApplyUnderTreeFilter>(ConvertTextToFilters(text));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::IsTrue(rebuilt->IncludeSelf);

         auto under = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Under);
         Assert::IsTrue(under != nullptr);
         Assert::AreEqual(L"a", under->Contained.c_str());

         auto subFilter = dynamic_pointer_cast<AcceptTreeFilter>(rebuilt->Filter);
         Assert::IsTrue(subFilter != nullptr);
      }

      TEST_METHOD(ConvertToTextRemoveChildrenFilter)
      {
         auto accept = NoChild(Contains(L"abc"));

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: no-child [ false, contains [ \"abc\" ] ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<RemoveChildrenTreeFilter>(ConvertTextToFilters(text));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::IsFalse(rebuilt->RemoveSelf);

         auto subFilter = dynamic_pointer_cast<ContainsTreeFilter>(rebuilt->Filter);
         Assert::IsTrue(subFilter != nullptr);
         Assert::AreEqual(L"abc", subFilter->Contained.c_str());
      }

      TEST_METHOD(ConvertToTextRangeFilter)
      {
         auto accept = LevelRange(7, 9);

         const wstring text = ConvertFiltersToText(accept);

         Assert::AreEqual(L"V1: range [ 7, 9 ]", text.c_str());

         auto rebuilt = dynamic_pointer_cast<LevelRangeTreeFilter>(ConvertTextToFilters(text));
         Assert::IsTrue(rebuilt != nullptr);

         Assert::AreEqual<size_t>(7, rebuilt->MinLevel);
         Assert::AreEqual<size_t>(9, rebuilt->MaxLevel);
      }

   private:

		shared_ptr<TextLines> CreateTextLines()
		{
			auto textLines = make_shared<TextLines>();

			textLines->push_back(L"abc");
			textLines->push_back(L"def");
			textLines->push_back(L"ghi");
			textLines->push_back(L"jkl");
			textLines->push_back(L"mno");
			textLines->push_back(L"pqr");
			textLines->push_back(L"stu");
			textLines->push_back(L"vwx");

			return textLines;
		}

		TextTree CreateSimpleTree()
		{
			TextTree textTree;

			textTree.SourceTextLines = CreateTextLines();

			textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(0), size_t(-1),  1 });
			textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(1),  2,  3 });
			textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(2), size_t(-1),  4 });
			textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(3), size_t(-1), size_t(-1) });
			textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(4), size_t(-1),  5 });
			textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(5),  6, size_t(-1) });
			textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(6), size_t(-1),  7 });
			textTree.Nodes.push_back(TextTree::Node{ &textTree.SourceTextLines->at(7), size_t(-1), size_t(-1) });

			return textTree;
		}
	};
}
