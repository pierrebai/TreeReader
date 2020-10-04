#include "dak/tree_reader/text_tree_visitor.h"

namespace dak::tree_reader
{
   using namespace std;
   using result = tree_visitor_t::result_t;
   using node = text_tree_t::node_t;
   using Iter = vector<node *>::const_iterator;
   using IterPair = pair<Iter, Iter>;
   constexpr result continue_visit{ false, false };
   constexpr result stop_visit{ true, false };

   result simple_tree_visitor_t::go_deeper(size_t deeperLevel)
   {
      return continue_visit;
   }

   result simple_tree_visitor_t::go_higher(size_t higherLevel)
   {
      return continue_visit;
   }

   result delegate_tree_visitor::visit(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
   {
      if (!visitor)
         return stop_visit;

      return visitor->visit(tree, node, level);
   }

   result function_tree_visitor_t::visit(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
   {
      return func(tree, node, level);
   }

   result can_abort_tree_visitor::visit(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
   {
      if (abort)
         return stop_visit;

      return delegate_tree_visitor::visit(tree, node, level);
   }

   void visit_in_order(const text_tree_t& tree, const node* a_node, bool siblings, tree_visitor_t& visitor)
   {
      IterPair pos;
      if (a_node)
      {
         if (siblings)
         {
            if (a_node->parent)
            {
               pos = make_pair(a_node->parent->children.begin(), a_node->parent->children.end());
            }
            else
            {
               pos = make_pair(tree.roots.begin(), tree.roots.end());
            }
            pos.first += a_node->index_in_parent;
         }
         else
         {
            pos = make_pair(a_node->children.begin(), a_node->children.end());
         }
      }
      else
      {
         pos = make_pair(tree.roots.begin(), tree.roots.end());
      }

      vector<IterPair> go_back;

      size_t level = 0;
      while (true)
      {
         if (pos.first != pos.second)
         {
            const node& c_node = **pos.first;
            const result result = visitor.visit(tree, c_node, level);
            if (result.stop)
               break;

            if (!result.skip_children && c_node.children.size() > 0)
            {
               go_back.push_back(pos);
               pos = make_pair(c_node.children.begin(), c_node.children.end());
               level++;
               if (visitor.go_deeper(level).stop)
                  break;
            }
            else
            {
               ++pos.first;
            }
         }
         else if (go_back.empty())
         {
            break;
         }
         else
         {
            pos = go_back.back();
            go_back.pop_back();
            ++pos.first;
            level--;
            if (visitor.go_higher(level).stop)
               break;
         }
      }
   }

   void visit_in_order(const text_tree_t& tree, const node* node, bool siblings, const node_visit_function_t& func)
   {
      function_tree_visitor_t visitor(func);
      visit_in_order(tree, node, siblings, visitor);
   }
}
