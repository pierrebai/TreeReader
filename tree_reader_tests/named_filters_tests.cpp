#include "dak/tree_reader/named_filters.h"

#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace dak::tree_reader;

namespace dak::tree_reader_tests
{
   TEST_CLASS(named_filters_tests)
   {
   public:

      TEST_METHOD(add_and_get_named_filter)
      {
         shared_ptr<tree_filter> filter = contains(L"abc");

         named_filters filters;
         filters.add(L"def", filter);

         Assert::AreEqual<size_t>(1, filters.all().size());
         Assert::IsNotNull(filters.get(L"def").get());
         Assert::IsTrue(filter == filters.get(L"def")->filter);
      }

      TEST_METHOD(save_and_load_named_filter)
      {
         named_filters filters;
         filters.add(L"def", contains(L"abc"));

         wstringstream sstream;
         save_named_filters(sstream, filters);

         named_filters new_filters = load_named_filters(sstream);

         Assert::AreEqual<size_t>(1, new_filters.all().size());
         Assert::IsTrue(new_filters.get(L"def") != nullptr);
         Assert::IsTrue(new_filters.get(L"def")->filter != nullptr);
         Assert::IsTrue(dynamic_pointer_cast<contains_tree_filter>(new_filters.get(L"def")->filter).get());
         Assert::AreEqual(wstring(L"abc"), dynamic_pointer_cast<contains_tree_filter>(new_filters.get(L"def")->filter)->contained);
      }
   };
}
