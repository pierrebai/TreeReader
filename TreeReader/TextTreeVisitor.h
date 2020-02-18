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
      virtual Result Visit(const TextTree::Node& node, size_t index, size_t level) = 0;
   };

   typedef std::function<TreeVisitor::Result(const TextTree::Node & node, size_t index, size_t level)> NodeVisitFunction;

   struct SimpleTreeVisitor : TreeVisitor
   {
      NodeVisitFunction Func;

      SimpleTreeVisitor(NodeVisitFunction f) : Func(f) {}

      Result GoDeeper(size_t deeperLevel) override { return Result(); }
      Result GoHigher(size_t higherLevel) override { return Result(); }
      Result Visit(const TextTree::Node& node, size_t index, size_t level) override
      {
         return Func(node, index, level);
      }
   };

   void VisitInOrder(const TextTree& tree, size_t index, TreeVisitor& visitor);
   void VisitInOrder(const TextTree& tree, size_t index, const NodeVisitFunction& func);
}
