#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace TreeReader
{
   // Holds whatever is necessary to make the raw text pointer valid for the lifetime of a text tree.

   struct TextHolder
   {
      virtual ~TextHolder() = default;
   };

   // The tree of text.
   //
   // Contains a vector of nodes, forming a tree structure.
   // Each node contains its text and the index of the next sibling and the first child.
   //
   // Use a TextHolder to make the text used by the tree nodes valid.

   struct TextTree
   {
      // Each node contains its text and the index of the next sibling and the first child.

      struct Node
      {
         // Index into the source text lines.
         const wchar_t* TextPtr = nullptr;

         // Index of parent node.
         size_t ParentIndex = size_t(-1);

         // Index of next node in the same parent.
         size_t NextSiblingIndex = size_t(-1);

         // Index into the node vector.
         size_t FirstChildIndex = size_t(-1);

         // Number of direct children.
         size_t ChildrenCount = 0;
      };

      // Source text lines are kept constant so that the text pointers are kept valid.
      std::shared_ptr<TextHolder> SourceTextLines;

      // The tree of nodes.
      std::vector<Node> Nodes;

      // Number of root nodes.
      size_t RootCount = 0;

      // Adding new nodes. To add the first root, pass any index on an empty tree.
      size_t AddChild(size_t underIndex, const wchar_t* text);
      size_t AddSibling(size_t afterIndex, const wchar_t* text);

      // Count the number of siblings, including the node itself.
      // Returns zero if the index is invalid.
      size_t CountSiblings(size_t fromIndex) const;

      // Count the number of children of the node.
      // Returns zero if the index is invalid.
      size_t CountChildren(size_t underIndex) const;

      // Count the number of ancestor to reach the root of the tree.
      // That is, root nodes have an ancestor count of zero.
      size_t CountAncestors(size_t fromIndex) const;

   private:
      size_t AddChild(size_t underIndex, size_t newIndex);
      size_t AddSibling(size_t afterIndex, size_t newIndex);
   };

   // Convert the text tree to a textual form with indentation.

   std::wostream& PrintTree(std::wostream& stream, const TextTree& tree, const std::wstring& indentation = L"  ");
   std::wostream& operator<<(std::wostream& stream, const TextTree& tree);
}
