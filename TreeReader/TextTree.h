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

         // Index of next node in the same parent.
         size_t NextSiblingIndex = size_t(-1);

         // Index into the node vector.
         size_t FirstChildIndex = size_t(-1);
      };

      // Source text lines are kept constant so that the text pointers are kept valid.
      std::shared_ptr<TextHolder> SourceTextLines;

      // The tree of nodes.
      std::vector<Node> Nodes;

      // Adding new nodes. To add the first root, pass any index on an empty tree.
      size_t AddChild(size_t index, const wchar_t* text);
      size_t AddSibling(size_t index, const wchar_t* text);

      // Count the number of siblings, including the node itself.
      // Returns zero if the index is invalid.
      size_t CountSiblings(size_t index) const;

      // Count the number of children of the node.
      // Returns zero if the index is invalid.
      size_t CountChildren(size_t index) const;

   private:
      size_t AddChild(size_t index, size_t newIndex);
      size_t AddSibling(size_t index, size_t newIndex);
   };

   // Convert the text tree to a textual form with indentation.

   std::wostream& operator<<(std::wostream& stream, const TextTree& tree);
}
