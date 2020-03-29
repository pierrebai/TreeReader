#pragma once

#include "dak/tree_reader/text_tree.h"

#include <functional>
#include <memory>
#include <atomic>

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A visitor that access each node of a tree one by one.

   struct tree_visitor
   {
      // Possible result of visiting a node: stop or not, skip children or not.

      struct result
      {
         bool stop = false;
         bool skip_children = false;
      };

      // Called when going deeper in the tree. (Before visitin gthe deeper nodes.)
      virtual result go_deeper(size_t deeperLevel) = 0;

      // Called when going higher in the tree. (Before visitin gthe higher nodes.)
      virtual result go_higher(size_t higherLevel) = 0;

      // Called when visiting a node.
      virtual result visit(const text_tree& tree, const text_tree::node& node, size_t level) = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Simple visitor that doesn't need to know that it is going deeper or higher.

   struct simple_tree_visitor : tree_visitor
   {
      result go_deeper(size_t deeperLevel) override;
      result go_higher(size_t higherLevel) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A visitor that calls another visitor.
   //
   // allows adding behavior to another existing visitor.

   struct delegate_tree_visitor : simple_tree_visitor
   {
      std::shared_ptr<tree_visitor> visitor;

      delegate_tree_visitor() = default;
      delegate_tree_visitor(const std::shared_ptr<tree_visitor>& visitor) : visitor(visitor) {}

      result visit(const text_tree& tree, const text_tree::node& node, size_t level) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A visitor that delegates to a function when visiting each node.

   typedef std::function<tree_visitor::result(const text_tree & tree, const text_tree::node & node, size_t level)> node_visit_function;

   struct function_tree_visitor : simple_tree_visitor
   {
      node_visit_function func;

      function_tree_visitor() = default;
      function_tree_visitor(node_visit_function f) : func(f) {}

      result visit(const text_tree& tree, const text_tree::node& node, size_t level) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A delegate visitor that can be aborted from another thread.

   struct can_abort_tree_visitor : delegate_tree_visitor
   {
      std::atomic<bool> abort = false;

      can_abort_tree_visitor() = default;
      can_abort_tree_visitor(const std::shared_ptr<tree_visitor> & visitor) : delegate_tree_visitor(visitor) {}

      result visit(const text_tree& tree, const text_tree::node& node, size_t level) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // visits each node of a tree in order.
   //
   // That is, visit each node before its children and visits its children before its siblings.
   //
   // allows starting from an arbitrary node and not visiting the siblings of that initial node.
   // (not visiting siblings also skip the initial node too.)

   void visit_in_order(const text_tree& tree, const text_tree::node* node, bool siblings, tree_visitor& visitor);
   void visit_in_order(const text_tree& tree, const text_tree::node* node, bool siblings, const node_visit_function& func);

   inline void visit_in_order(const text_tree& tree, const text_tree::node* node, tree_visitor& visitor)
   {
      visit_in_order(tree, node, true, visitor);
   }

   inline void visit_in_order(const text_tree& tree, const text_tree::node* node, const node_visit_function& func)
   {
      visit_in_order(tree, node, true, func);
   }

   inline void visit_in_order(const text_tree& tree, tree_visitor& visitor)
   {
      visit_in_order(tree, nullptr, true, visitor);
   }

   inline void visit_in_order(const text_tree& tree, const node_visit_function& func)
   {
      visit_in_order(tree, nullptr, true, func);
   }
}
