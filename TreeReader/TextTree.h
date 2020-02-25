#pragma once

#include <string>
#include <deque>
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
   // Contains nodes, forming a tree structure.
   // Each node contains its text, children and index in its parent.
   //
   // Use a TextHolder to make the text used by the tree nodes valid.

   struct TextTree
   {
      // Each node contains its text and the index of the next sibling and the first child.

      struct Node
      {
         // Points into the source text lines.
         const wchar_t* TextPtr = nullptr;

         Node* Parent = nullptr;

         size_t IndexInParent = 0;

         std::vector<Node *> Children;

         Node() = default;
         Node(const wchar_t* text, Node* parent) : TextPtr(text), Parent(parent) {}
      };

      // Source text lines are kept constant so that the text pointers are kept valid.
      std::shared_ptr<TextHolder> SourceTextLines;

      // The roots of the tree of nodes.
      std::vector<Node *> Roots;

      // Clear the tree.
      void Reset();

      // Adding new nodes. To add the a root, pass nullptr.
      Node* AddChild(Node* underNode, const wchar_t* text);

      // Count the number of chilren of a node.
      // Pass null to count the number of roots.
      size_t CountChildren(const Node* node) const;

      // Count the number of siblings, including the node itself.
      // Returns zero if the node is null.
      size_t CountSiblings(const Node* node) const;

      // Count the number of ancestor to reach the root of the tree.
      // That is, root nodes have an ancestor count of zero.
      size_t CountAncestors(const Node* node) const;

   private:
      // The nodes.
      std::deque<Node> _nodes;

   };

   // Convert the text tree to a textual form with indentation.

   std::wostream& PrintTree(std::wostream& stream, const TextTree& tree, const std::wstring& indentation = L"  ");
   std::wostream& operator<<(std::wostream& stream, const TextTree& tree);
}
