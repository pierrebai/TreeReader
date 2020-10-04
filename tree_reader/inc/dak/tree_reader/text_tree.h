#pragma once

#include <string>
#include <deque>
#include <vector>
#include <memory>
#include <iostream>

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Holds whatever is necessary to make the raw text pointer valid
   // for the lifetime of a text tree.

   struct text_holder_t
   {
      virtual ~text_holder_t() = default;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // The tree of text.
   //
   // contains nodes, forming a tree structure.
   // Each node contains its text, children and index in its parent.
   //
   // Use a text_holder to make the text used by the tree nodes valid.

   struct text_tree_t
   {
      // Each node contains its text and the index of the next sibling and the first child.

      struct node_t
      {
         // Points into the source text lines.
         const wchar_t* text_ptr = nullptr;

         node_t* parent = nullptr;

         size_t index_in_parent = 0;

         std::vector<node_t *> children;

         node_t() = default;
         node_t(const wchar_t* text, node_t* parent) : text_ptr(text), parent(parent) {}
      };

      // Source text lines are kept constant so that the text pointers are kept valid.
      std::shared_ptr<text_holder_t> source_text_lines;

      // The roots of the tree of nodes.
      std::vector<node_t *> roots;

      // clear the tree.
      void reset();

      // adding new nodes. To add the a root, pass nullptr.
      node_t* add_child(node_t* undernode, const wchar_t* text);

      // Count the number of chilren of a node.
      // Pass null to count the number of roots.
      size_t count_children(const node_t* node) const;

      // Count the number of siblings, including the node itself.
      // Returns zero if the node is null.
      size_t count_siblings(const node_t* node) const;

      // Count the number of ancestor to reach the root of the tree.
      // That is, root nodes have an ancestor count of zero.
      size_t count_ancestors(const node_t* node) const;

   private:
      // The nodes.
      std::deque<node_t> _nodes;

   };

   typedef std::shared_ptr<text_tree_t> text_tree_ptr_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Convert the text tree to a textual form with indentation.

   std::wostream& print_tree(std::wostream& stream, const text_tree_t& tree, const std::wstring& indentation = L"  ");
   std::wostream& operator<<(std::wostream& stream, const text_tree_t& tree);
}
