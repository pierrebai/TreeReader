#include "dak/tree_reader/text_tree.h"

namespace dak::tree_reader
{
   using namespace std;

   void text_tree::reset()
   {
      roots.clear();
      _nodes.clear();
   }

   text_tree::node* text_tree::add_child(node* undernode, const wchar_t* text)
   {
      _nodes.emplace_back(text, undernode);
      node* newnode = &_nodes.back();
      if (!undernode)
      {
         newnode->index_in_parent = roots.size();
         roots.emplace_back(newnode);
      }
      else
      {
         newnode->index_in_parent = undernode->children.size();
         undernode->children.emplace_back(newnode);
      }
      return newnode;
   }

   size_t text_tree::count_siblings(const node* node) const
   {
      if (!node)
         return 0;

      return count_children(node->parent);
   }

   size_t text_tree::count_children(const node* node) const
   {
      if (!node)
         return roots.size();

      return node->children.size();
   }

   size_t text_tree::count_ancestors(const node* node) const
   {
      if (!node)
         return 0;

      size_t count = 0;

      while (node->parent)
      {
         count += 1;
         node = node->parent;
      }

      return count;
   }
}
