#include "TextTreeVisitor.h"

namespace TreeReader
{
   using namespace std;
   using Result = TreeVisitor::Result;
   using Node = TextTree::Node;
   using Iter = vector<Node *>::const_iterator;
   using IterPair = pair<Iter, Iter>;
   constexpr Result ContinueVisit{ false, false };
   constexpr Result StopVisit{ true, false };

   Result SimpleTreeVisitor::GoDeeper(size_t deeperLevel)
   {
      return ContinueVisit;
   }

   Result SimpleTreeVisitor::GoHigher(size_t higherLevel)
   {
      return ContinueVisit;
   }

   Result DelegateTreeVisitor::Visit(const TextTree& tree, const TextTree::Node& node, size_t level)
   {
      if (!Visitor)
         return StopVisit;

      return Visitor->Visit(tree, node, level);
   }

   Result FunctionTreeVisitor::Visit(const TextTree& tree, const TextTree::Node& node, size_t level)
   {
      return Func(tree, node, level);
   }

   Result CanAbortTreeVisitor::Visit(const TextTree& tree, const TextTree::Node& node, size_t level)
   {
      if (Abort)
         return StopVisit;

      return DelegateTreeVisitor::Visit(tree, node, level);
   }

   void VisitInOrder(const TextTree& tree, const Node* node, bool siblings, TreeVisitor& visitor)
   {
      IterPair pos;
      if (node)
      {
         if (siblings)
         {
            if (node->Parent)
            {
               pos = make_pair(node->Parent->Children.begin(), node->Parent->Children.end());
            }
            else
            {
               pos = make_pair(tree.Roots.begin(), tree.Roots.end());
            }
            pos.first += node->IndexInParent;
         }
         else
         {
            pos = make_pair(node->Children.begin(), node->Children.end());
         }
      }
      else
      {
         pos = make_pair(tree.Roots.begin(), tree.Roots.end());
      }

      vector<IterPair> goBack;

      size_t level = 0;
      while (true)
      {
         if (pos.first != pos.second)
         {
            const Node& node = **pos.first;
            const Result result = visitor.Visit(tree, node, level);
            if (result.Stop)
               break;

            if (!result.SkipChildren && node.Children.size() > 0)
            {
               goBack.push_back(pos);
               pos = make_pair(node.Children.begin(), node.Children.end());
               level++;
               if (visitor.GoDeeper(level).Stop)
                  break;
            }
            else
            {
               ++pos.first;
            }
         }
         else if (goBack.empty())
         {
            break;
         }
         else
         {
            pos = goBack.back();
            goBack.pop_back();
            ++pos.first;
            level--;
            if (visitor.GoHigher(level).Stop)
               break;
         }
      }
   }

   void VisitInOrder(const TextTree& tree, const Node* node, bool siblings, const NodeVisitFunction& func)
   {
      FunctionTreeVisitor visitor(func);
      VisitInOrder(tree, node, siblings, visitor);
   }
}
