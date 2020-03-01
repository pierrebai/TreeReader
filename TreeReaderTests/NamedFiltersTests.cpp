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
         filters.Add(L"def", filter);

         Assert::AreEqual<size_t>(1, filters.All().size());
         Assert::IsNotNull(filters.Get(L"def").get());
         Assert::IsTrue(filter == filters.Get(L"def")->Filter);
      }

      TEST_METHOD(SaveAndLoadNamedFilter)
      {
         NamedFilters filters;
         filters.Add(L"def", Contains(L"abc"));

         wstringstream sstream;
         WriteNamedFilters(sstream, filters);

         NamedFilters newFilters = ReadNamedFilters(sstream);

         Assert::AreEqual<size_t>(1, newFilters.All().size());
         Assert::IsTrue(newFilters.Get(L"def") != nullptr);
         Assert::IsTrue(newFilters.Get(L"def")->Filter != nullptr);
         Assert::IsTrue(dynamic_pointer_cast<ContainsTreeFilter>(newFilters.Get(L"def")->Filter).get());
         Assert::AreEqual(wstring(L"abc"), dynamic_pointer_cast<ContainsTreeFilter>(newFilters.Get(L"def")->Filter)->Contained);
      }
   };
}
