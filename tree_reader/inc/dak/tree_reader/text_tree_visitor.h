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

   struct tree_visitor_t
   {
      // Possible result of visiting a node: stop or not, skip children or not.

      struct result_t
      {
         bool stop = false;
         bool skip_children = false;
      };

      // Called when going deeper in the tree. (Before visitin gthe deeper nodes.)
      virtual result_t go_deeper(size_t deeperLevel) = 0;

      // Called when going higher in the tree. (Before visitin gthe higher nodes.)
      virtual result_t go_higher(size_t higherLevel) = 0;

      // Called when visiting a node.
      virtual result_t visit(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level) = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Simple visitor that doesn't need to know that it is going deeper or higher.

   struct simple_tree_visitor_t : tree_visitor_t
   {
      result_t go_deeper(size_t deeperLevel) override;
      result_t go_higher(size_t higherLevel) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A visitor that calls another visitor.
   //
   // allows adding behavior to another existing visitor.

   struct delegate_tree_visitor : simple_tree_visitor_t
   {
      std::shared_ptr<tree_visitor_t> visitor;

      delegate_tree_visitor() = default;
      delegate_tree_visitor(const std::shared_ptr<tree_visitor_t>& visitor) : visitor(visitor) {}

      result_t visit(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A visitor that delegates to a function when visiting each node.

   typedef std::function<tree_visitor_t::result_t(const text_tree_t & tree, const text_tree_t::node_t & node, size_t level)> node_visit_function_t;

   struct function_tree_visitor_t : simple_tree_visitor_t
   {
      node_visit_function_t func;

      function_tree_visitor_t() = default;
      function_tree_visitor_t(node_visit_function_t f) : func(f) {}

      result_t visit(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A delegate visitor that can be aborted from another thread.

   struct can_abort_tree_visitor : delegate_tree_visitor
   {
      std::atomic<bool> abort = false;

      can_abort_tree_visitor() = default;
      can_abort_tree_visitor(const std::shared_ptr<tree_visitor_t> & visitor) : delegate_tree_visitor(visitor) {}

      result_t visit(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // visits each node of a tree in order.
   //
   // That is, visit each node before its children and visits its children before its siblings.
   //
   // allows starting from an arbitrary node and not visiting the siblings of that initial node.
   // (not visiting siblings also skip the initial node too.)

   void visit_in_order(const text_tree_t& tree, const text_tree_t::node_t* node, bool siblings, tree_visitor_t& visitor);
   void visit_in_order(const text_tree_t& tree, const text_tree_t::node_t* node, bool siblings, const node_visit_function_t& func);

   inline void visit_in_order(const text_tree_t& tree, const text_tree_t::node_t* node, tree_visitor_t& visitor)
   {
      visit_in_order(tree, node, true, visitor);
   }

   inline void visit_in_order(const text_tree_t& tree, const text_tree_t::node_t* node, const node_visit_function_t& func)
   {
      visit_in_order(tree, node, true, func);
   }

   inline void visit_in_order(const text_tree_t& tree, tree_visitor_t& visitor)
   {
      visit_in_order(tree, nullptr, true, visitor);
   }

   inline void visit_in_order(const text_tree_t& tree, const node_visit_function_t& func)
   {
      visit_in_order(tree, nullptr, true, func);
   }
}
