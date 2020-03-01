#include "TreeFilter.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace TreeReaderTests
{
	TEST_CLASS(TreeFilterAcceptTests)
	{
	public:

      TEST_METHOD(AcceptFilterAccept)
      {
         auto filter = Accept();

         Assert::IsFalse(filter->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsFalse(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(StopFilterAccept)
      {
         auto filter = Stop();

         Assert::IsFalse(filter->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsFalse(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(UntilFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = Until(sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(ContainsFilterAccept)
      {
         auto filter = Contains(L"g");

         Assert::IsFalse(filter->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsFalse(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(ExactAddressFilterAccept)
      {
         auto filter = ExactAddress(L"g");

         Assert::IsFalse(filter->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsFalse (filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(RegexFilterAccept)
      {
         auto filter = Regex(L"g");

         Assert::IsFalse(filter->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsFalse(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(NotFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = Not(sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(OrFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = Or(Accept(), sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsTrue(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsTrue(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsTrue(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(AndFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = And(Accept(), sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsTrue(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsTrue(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsTrue(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(AnyFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = Any({ sub });

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsTrue(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsTrue(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsTrue(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(AllFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = All({ sub });

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsTrue(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsTrue(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsTrue(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(UnderFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = Under(sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(CountSiblingsFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = CountSiblings(sub, 2);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(CountChildrenFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = CountChildren(sub, 3);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(NoChildFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = NoChild(sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(LevelRangeFilterAccept)
      {
         auto filter = LevelRange(2, 4);

         Assert::IsFalse(filter->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsFalse(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(IfSubTreeFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = IfSubTree(sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(IfSiblingFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = IfSibling(sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsTrue(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }

      TEST_METHOD(NamedFilterAccept)
      {
         auto sub = Not(Stop());
         auto filter = Named(L"g", sub);

         Assert::IsFalse(filter->CanAccept(filter));
         Assert::IsFalse(filter->CanAccept(sub));
         Assert::IsFalse(sub->CanAccept(filter));

         Assert::IsFalse(filter->CanAccept(Accept()));
         Assert::IsFalse(filter->CanAccept(Not(nullptr)));
         Assert::IsFalse(filter->CanAccept(Not(Contains(L"a"))));
      }
   };
}
