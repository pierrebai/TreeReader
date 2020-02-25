#pragma once

#include "TextTree.h"

#include <functional>

namespace TreeReader
{
   // A visitor that access each node of a tree one by one.

   struct TreeVisitor
   {
      // Possible result of visiting a node: stop or not, skip children or not.

      struct Result
      {
         bool Stop = false;
         bool SkipChildren = false;
      };

      // Called when going deeper in the tree. (Before visitin gthe deeper nodes.)
      virtual Result GoDeeper(size_t deeperLevel) = 0;

      // Called when going higher in the tree. (Before visitin gthe higher nodes.)
      virtual Result GoHigher(size_t higherLevel) = 0;

      // Called when visiting a node.
      virtual Result Visit(const TextTree& tree, const TextTree::Node& node, size_t level) = 0;
   };

   // Simple visitor that doesn't need to know that it is going deeper or higher.

   struct SimpleTreeVisitor : TreeVisitor
   {
      Result GoDeeper(size_t deeperLevel) override { return Result(); }
      Result GoHigher(size_t higherLevel) override { return Result(); }
   };

   // A visitor that delegates to a function when visiting each node.

   typedef std::function<TreeVisitor::Result(const TextTree & tree, const TextTree::Node & node, size_t level)> NodeVisitFunction;

   struct FunctionTreeVisitor : SimpleTreeVisitor
   {
      NodeVisitFunction Func;

      FunctionTreeVisitor(NodeVisitFunction f) : Func(f) {}

      Result Visit(const TextTree& tree, const TextTree::Node& node, size_t level) override
      {
         return Func(tree, node, level);
      }
   };

   // Visits each node of a tree in order.
   //
   // That is, visit each node before its children and visits its children before its siblings.
   //
   // Allows starting from an arbitrary node and not visiting the siblings of that initial node.

   void VisitInOrder(const TextTree& tree, const TextTree::Node* node, bool siblings, TreeVisitor& visitor);
   void VisitInOrder(const TextTree& tree, const TextTree::Node* node, bool siblings, const NodeVisitFunction& func);

   inline void VisitInOrder(const TextTree& tree, const TextTree::Node* node, TreeVisitor& visitor)
   {
      VisitInOrder(tree, node, true, visitor);
   }

   inline void VisitInOrder(const TextTree& tree, const TextTree::Node* node, const NodeVisitFunction& func)
   {
      VisitInOrder(tree, node, true, func);
   }

   inline void VisitInOrder(const TextTree& tree, TreeVisitor& visitor)
   {
      VisitInOrder(tree, nullptr, true, visitor);
   }

   inline void VisitInOrder(const TextTree& tree, const NodeVisitFunction& func)
   {
      VisitInOrder(tree, nullptr, true, func);
   }
}
