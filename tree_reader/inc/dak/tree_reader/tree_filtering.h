#pragma once

#include "dak/tree_reader/text_tree_visitor.h"

#include <memory>
#include <vector>
#include <future>

namespace dak::tree_reader
{
   struct tree_filter;
   typedef std::shared_ptr<tree_filter> tree_filter_ptr;

   ////////////////////////////////////////////////////////////////////////////
   //
   // named_filters a source tree into a filtered tree using the given filter.

   void filter_tree(const text_tree& sourceTree, text_tree& filteredTree, tree_filter& filter);
   void filter_tree(const text_tree& sourceTree, text_tree& filteredTree, const tree_filter_ptr& filter);

   using async_filter_tree_result = std::pair<std::future<text_tree>, std::shared_ptr<can_abort_tree_visitor>>;

   async_filter_tree_result filter_tree_async(const text_tree_ptr& sourceTree, const tree_filter_ptr& filter);

   ////////////////////////////////////////////////////////////////////////////
   //
   // The tree visitor that actually does the filtering.

   struct filter_tree_visitor : simple_tree_visitor
   {
      text_tree& filtered_tree;
      tree_filter& filter;

      filter_tree_visitor(const text_tree& sourceTree, text_tree& filteredTree, tree_filter& filter);

      result visit(const text_tree& tree, const text_tree::node& sourcenode, const size_t sourceLevel) override;

   private:
      // This keeps the current branch of nodes we have created.
      // We will keep one entry per source level, even when some
      // levels were filtered out.
      std::vector<text_tree::node*> _filtered_branch_nodes;
      std::vector<bool> _fill_children;
   };

}

