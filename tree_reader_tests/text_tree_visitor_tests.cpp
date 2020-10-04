#include "dak/tree_reader/text_tree_visitor.h"
#include "tree_reader_test_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace dak::tree_reader;

namespace dak::tree_reader_tests
{
   TEST_CLASS(text_tree_visitor_tests)
   {
   public:

      TEST_METHOD(visitEmptyTree)
      {
         size_t visits = 0;
         visit_in_order(text_tree_t(), function_tree_visitor_t([&visits](const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
         {
            visits += 1;
            return tree_visitor_t::result_t();
         }));

         Assert::AreEqual<size_t>(0, visits);
      }

      TEST_METHOD(visitSimpleTree)
      {
         size_t visits = 0;
         visit_in_order(create_simple_tree(), function_tree_visitor_t([&visits](const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
         {
            visits += 1;
            return tree_visitor_t::result_t();
         }));

         Assert::AreEqual<size_t>(8, visits);
      }

      TEST_METHOD(visitSimpleTreeWithDelegate)
      {
         size_t visits = 0;
         auto visitor = make_shared<function_tree_visitor_t>([&visits](const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
         {
            visits += 1;
            return tree_visitor_t::result_t();
         });
         
         visit_in_order(create_simple_tree(), delegate_tree_visitor(visitor));

         Assert::AreEqual<size_t>(8, visits);
      }

      TEST_METHOD(visitSimpleTreeWithabort)
      {
         size_t visits = 0;
         auto visitor = make_shared<function_tree_visitor_t>([&visits](const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
         {
            visits += 1;
            return tree_visitor_t::result_t();
         });

         can_abort_tree_visitor abort(visitor);
         abort.abort = true;

         visit_in_order(create_simple_tree(), abort);

         Assert::AreEqual<size_t>(0, visits);
      }

   };
}
