#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace TreeReader
{
   typedef std::wstring Text;
   typedef std::vector<Text> TextLines;

   struct TextTree
   {
      struct Node
      {
         // Index into the source text lines.
         const Text* TextPtr = nullptr;

         // Index of next node in the same parent.
         size_t NextSiblingIndex = size_t(-1);

         // Index into the node vector.
         size_t FirstChildIndex = size_t(-1);
      };

      // Source text lines are kept constant so that the text pointers are kept valid.
      std::shared_ptr<const TextLines> SourceTextLines;

      // The tree of nodes.
      std::vector<Node> Nodes;

      // Adding new nodes. To add the first root, pass any index on an empty tree.
      size_t AddChild(size_t index, const Text* TextPtr);
      size_t AddSibling(size_t index, const Text* TextPtr);

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

   std::wostream& operator<<(std::wostream& stream, const TextTree& tree);
}
