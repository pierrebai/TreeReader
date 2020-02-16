#pragma once

#include "TextTree.h"

#include <functional>

namespace TreeReader
{
   struct TreeVisitor
   {
      virtual bool GoDeeper(size_t deeperLevel) = 0;
      virtual bool GoHigher(size_t higherLevel) = 0;
      virtual bool Visit(const TextTree::Node& node, size_t index, size_t level) = 0;
   };

   typedef std::function<bool(const TextTree::Node & node, size_t index, size_t level)> NodeVisitFunction;

   struct SimpleTreeVisitor : TreeVisitor
   {
      NodeVisitFunction Func;

      SimpleTreeVisitor(NodeVisitFunction f) : Func(f) {}

      bool GoDeeper(size_t deeperLevel) override { return true; }
      bool GoHigher(size_t higherLevel) override { return true; }
      bool Visit(const TextTree::Node& node, size_t index, size_t level) override
      {
         return Func(node, index, level);
      }
   };

   void VisitInOrder(const TextTree& tree, size_t index, TreeVisitor& visitor);
   void VisitInOrder(const TextTree& tree, size_t index, const NodeVisitFunction& func);
}
