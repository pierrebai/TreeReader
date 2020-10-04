#include "dak/tree_reader/tree_filtering.h"
#include "dak/tree_reader/tree_filter.h"

namespace dak::tree_reader
{
   using namespace std;
   using result = tree_filter_t::result_t;
   using node = text_tree_t::node_t;

   filter_tree_visitor_t::filter_tree_visitor_t(const text_tree_t& source_tree, text_tree_t& a_filtered_tree, tree_filter_t& filter)
      : filtered_tree(a_filtered_tree), filter(filter)
   {
      a_filtered_tree.reset();
      a_filtered_tree.source_text_lines = source_tree.source_text_lines;

      // To make the algorithm work the same way for the first node
      // we pretend that we've seen a preceeding sibling of the level
      // zero and did not keep it.
      //
      // So the current filtered branch for level zero is null.
      _filtered_branch_nodes.push_back(nullptr);
      _fill_children.push_back(false);
   }

   tree_visitor_t::result_t filter_tree_visitor_t::visit(const text_tree_t& tree, const node& source_node, const size_t source_level)
   {
      _filtered_branch_nodes.resize(source_level + 1, nullptr);
      _fill_children.resize(source_level + 1, false);

      // Either the index of the newly created filtered node if kept, or -1 if not kept.
      node* filtered_node = nullptr;

      const tree_filter_t::result_t result = filter.is_kept(tree, source_node, source_level);
      if (result.keep)
      {
         // Connect to the nearest node in the branch.
         node* add_under = nullptr;
         for (size_t level = source_level; level < _filtered_branch_nodes.size(); --level)
         {
            if (_filtered_branch_nodes[level])
            {
               // If the node is at the same level, do not add as a child.
               add_under = (level < source_level && _fill_children[level]) ? _filtered_branch_nodes[level] : _filtered_branch_nodes[level]->parent;
               break;
            }
         }

         filtered_node = filtered_tree.add_child(add_under, source_node.text_ptr);
      }

      // If kept, this node is the new active node for this level.
      // If not kept, do not over-write a sibling node that may exists at this level.
      _filtered_branch_nodes.resize(source_level + 1, nullptr);
      if (filtered_node)
         _filtered_branch_nodes[source_level] = filtered_node;

      // If the node is kept, start to add sub-node as children.
      // If not kept, make any existing singling node begin to add node as sibling instead
      // of children.
      _fill_children.resize(source_level + 1, false);
      _fill_children[source_level] = (filtered_node != nullptr);

      // note: we really do want to slice the result down to the tree_visitor::result type.
      return tree_visitor_t::result_t(result);
   }

   void filter_tree(const text_tree_t& source_tree, text_tree_t& filteredTree, tree_filter_t& filter)
   {
      filter_tree_visitor_t visitor(source_tree, filteredTree, filter);
      visit_in_order(source_tree, visitor);
   }

   void filter_tree(const text_tree_t& source_tree, text_tree_t& filteredTree, const tree_filter_ptr_t& filter)
   {
      if (!filter)
      {
         filteredTree = source_tree;
         return;
      }

      filter_tree(source_tree, filteredTree, *filter);
   }

   async_filter_tree_result_t filter_tree_async(const text_tree_ptr_t& source_tree, const tree_filter_ptr_t& filter)
   {
      if (!filter)
         return {};

      auto abort = make_shared<can_abort_tree_visitor>();
      auto fut = async(launch::async, [source_tree, filter, abort]()
      {
         text_tree_t filtered;
         abort->visitor = make_shared<filter_tree_visitor_t>(*source_tree, filtered, *filter);
         visit_in_order(*source_tree, *abort);
         return filtered;
      });

      return make_pair(move(fut), abort);
   }
}
