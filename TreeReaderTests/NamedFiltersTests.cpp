#include "NamedFilters.h"
#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace TreeReaderTests
{
   TEST_CLASS(NamedFiltersTests)
   {
   public:

      TEST_METHOD(AddAndGetNamedFilter)
      {
         shared_ptr<TreeFilter> filter = Contains(L"abc");

         NamedFilters filters;
         filters.Filters[L"def"] = filter;

         Assert::AreEqual<size_t>(1, filters.Filters.size());
         Assert::IsTrue(filter == filters.Get(L"def"));
      }

      TEST_METHOD(SaveAndLoadNamedFilter)
      {
         shared_ptr<TreeFilter> filter = Contains(L"abc");

         NamedFilters filters;
         filters.Filters[L"def"] = filter;

         wstringstream sstream;
         WriteNamedFilters(sstream, filters);

         NamedFilters newFilters = ReadNamedFilters(sstream);

         Assert::AreEqual<size_t>(1, newFilters.Filters.size());
         Assert::IsTrue(newFilters.Get(L"def") != nullptr);

         Assert::IsTrue(dynamic_pointer_cast<ContainsTreeFilter>(newFilters.Get(L"def")).get());
         Assert::AreEqual(wstring(L"abc"), dynamic_pointer_cast<ContainsTreeFilter>(newFilters.Get(L"def"))->Contained);
      }
   };
}
