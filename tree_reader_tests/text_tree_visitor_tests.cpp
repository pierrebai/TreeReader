#include "TextTreeVisitor.h"
#include "TreeReaderTestHelpers.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TreeReader;

namespace TreeReaderTests
{
   TEST_CLASS(TextTreeVisitorTests)
   {
   public:

      TEST_METHOD(VisitEmptyTree)
      {
         size_t visits = 0;
         VisitInOrder(TextTree(), FunctionTreeVisitor([&visits](const TextTree& tree, const TextTree::Node& node, size_t level)
         {
            visits += 1;
            return TreeVisitor::Result();
         }));

         Assert::AreEqual<size_t>(0, visits);
      }

      TEST_METHOD(VisitSimpleTree)
      {
         size_t visits = 0;
         VisitInOrder(CreateSimpleTree(), FunctionTreeVisitor([&visits](const TextTree& tree, const TextTree::Node& node, size_t level)
         {
            visits += 1;
            return TreeVisitor::Result();
         }));

         Assert::AreEqual<size_t>(8, visits);
      }

      TEST_METHOD(VisitSimpleTreeWithDelegate)
      {
         size_t visits = 0;
         auto visitor = make_shared<FunctionTreeVisitor>([&visits](const TextTree& tree, const TextTree::Node& node, size_t level)
         {
            visits += 1;
            return TreeVisitor::Result();
         });
         
         VisitInOrder(CreateSimpleTree(), DelegateTreeVisitor(visitor));

         Assert::AreEqual<size_t>(8, visits);
      }

      TEST_METHOD(VisitSimpleTreeWithAbort)
      {
         size_t visits = 0;
         auto visitor = make_shared<FunctionTreeVisitor>([&visits](const TextTree& tree, const TextTree::Node& node, size_t level)
         {
            visits += 1;
            return TreeVisitor::Result();
         });

         CanAbortTreeVisitor abort(visitor);
         abort.Abort = true;

         VisitInOrder(CreateSimpleTree(), abort);

         Assert::AreEqual<size_t>(0, visits);
      }

   };
}
