#pragma once

#include "dak/tree_reader/text_tree_visitor.h"

#include <memory>
#include <vector>
#include <future>

namespace dak::tree_reader
{
   struct tree_filter_t;
   typedef std::shared_ptr<tree_filter_t> tree_filter_ptr_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // named_filters a source tree into a filtered tree using the given filter.

   void filter_tree(const text_tree_t& sourceTree, text_tree_t& filteredTree, tree_filter_t& filter);
   void filter_tree(const text_tree_t& sourceTree, text_tree_t& filteredTree, const tree_filter_ptr_t& filter);

   using async_filter_tree_result_t = std::pair<std::future<text_tree_t>, std::shared_ptr<can_abort_tree_visitor>>;

   async_filter_tree_result_t filter_tree_async(const text_tree_ptr_t& sourceTree, const tree_filter_ptr_t& filter);

   ////////////////////////////////////////////////////////////////////////////
   //
   // The tree visitor that actually does the filtering.

   struct filter_tree_visitor_t : simple_tree_visitor_t
   {
      text_tree_t& filtered_tree;
      tree_filter_t& filter;

      filter_tree_visitor_t(const text_tree_t& sourceTree, text_tree_t& filteredTree, tree_filter_t& filter);

      result_t visit(const text_tree_t& tree, const text_tree_t::node_t& sourcenode, const size_t sourceLevel) override;

   private:
      // This keeps the current branch of nodes we have created.
      // We will keep one entry per source level, even when some
      // levels were filtered out.
      std::vector<text_tree_t::node_t*> _filtered_branch_nodes;
      std::vector<bool> _fill_children;
   };

}

