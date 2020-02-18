#include "TextTreeVisitor.h"

namespace TreeReader
{
   using namespace std;
   using Result = TreeVisitor::Result;

   void VisitInOrder(const TextTree& tree, size_t index, TreeVisitor& visitor)
   {
      if (index >= tree.Nodes.size())
         return;

      vector<size_t> goBack;

      size_t level = 0;
      while (true)
      {
         if (index != -1)
         {
            const TextTree::Node& node = tree.Nodes[index];

            const Result result = visitor.Visit(node, index, level);
            if (result.Stop)
               break;

            if (!result.SkipChildren && node.FirstChildIndex != -1)
            {
               goBack.push_back(node.NextSiblingIndex);
               index = node.FirstChildIndex;
               level++;
               if (visitor.GoDeeper(level).Stop)
                  break;
            }
            else
            {
               index = node.NextSiblingIndex;
            }
         }
         else if (goBack.empty())
         {
            break;
         }
         else
         {
            index = goBack.back();
            goBack.pop_back();
            level--;
            if (visitor.GoHigher(level).Stop)
               break;
         }
      }
   }

   void VisitInOrder(const TextTree& tree, size_t index, const NodeVisitFunction& func)
   {
      SimpleTreeVisitor visitor(func);
      VisitInOrder(tree, index, visitor);
   }
}
