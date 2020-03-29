#include "TextTree.h"

namespace TreeReader
{
   using namespace std;

   void TextTree::Reset()
   {
      Roots.clear();
      _nodes.clear();
   }

   TextTree::Node* TextTree::AddChild(Node* underNode, const wchar_t* text)
   {
      _nodes.emplace_back(text, underNode);
      Node* newNode = &_nodes.back();
      if (!underNode)
      {
         newNode->IndexInParent = Roots.size();
         Roots.emplace_back(newNode);
      }
      else
      {
         newNode->IndexInParent = underNode->Children.size();
         underNode->Children.emplace_back(newNode);
      }
      return newNode;
   }

   size_t TextTree::CountSiblings(const Node* node) const
   {
      if (!node)
         return 0;

      return CountChildren(node->Parent);
   }

   size_t TextTree::CountChildren(const Node* node) const
   {
      if (!node)
         return Roots.size();

      return node->Children.size();
   }

   size_t TextTree::CountAncestors(const Node* node) const
   {
      if (!node)
         return 0;

      size_t count = 0;

      while (node->Parent)
      {
         count += 1;
         node = node->Parent;
      }

      return count;
   }
}
