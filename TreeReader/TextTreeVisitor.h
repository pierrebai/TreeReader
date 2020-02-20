#pragma once

#include "TextTree.h"

#include <functional>

namespace TreeReader
{
   struct TreeVisitor
   {
      struct Result
      {
         bool Stop = false;
         bool SkipChildren = false;
      };

      virtual Result GoDeeper(size_t deeperLevel) = 0;
      virtual Result GoHigher(size_t higherLevel) = 0;
      virtual Result Visit(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level) = 0;
   };

   struct SimpleTreeVisitor : TreeVisitor
   {
      Result GoDeeper(size_t deeperLevel) override { return Result(); }
      Result GoHigher(size_t higherLevel) override { return Result(); }
   };

   typedef std::function<TreeVisitor::Result(const TextTree & tree, const TextTree::Node & node, size_t index, size_t level)> NodeVisitFunction;

   struct FunctionTreeVisitor : SimpleTreeVisitor
   {
      NodeVisitFunction Func;

      FunctionTreeVisitor(NodeVisitFunction f) : Func(f) {}

      Result Visit(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level) override
      {
         return Func(tree, node, index, level);
      }
   };

   void VisitInOrder(const TextTree& tree, size_t index, bool siblings, TreeVisitor& visitor);
   void VisitInOrder(const TextTree& tree, size_t index, bool siblings, const NodeVisitFunction& func);

   inline void VisitInOrder(const TextTree& tree, size_t index, TreeVisitor& visitor)
   {
      VisitInOrder(tree, index, true, visitor);
   }

   inline void VisitInOrder(const TextTree& tree, size_t index, const NodeVisitFunction& func)
   {
      VisitInOrder(tree, index, true, func);
   }
}
