#include "TextTree.h"
#include "TextTreeVisitor.h"

namespace TreeReader
{
   using namespace std;

   size_t TextTree::AddChild(size_t index, const Text* TextPtr)
   {
      const size_t newIndex = Nodes.size();
      Nodes.push_back(Node{ TextPtr });

      if (index >= Nodes.size() - 1)
         return newIndex;

      return AddChild(index, newIndex);
   }

   size_t TextTree::AddChild(size_t index, size_t newIndex)
   {
      if (Nodes[index].FirstChildIndex == -1)
      {
         Nodes[index].FirstChildIndex = newIndex;
         return newIndex;
      }
      else
      {
         return AddSibling(Nodes[index].FirstChildIndex, newIndex);
      }
   }

   size_t TextTree::AddSibling(size_t index, const Text* TextPtr)
   {
      const size_t newIndex = Nodes.size();
      Nodes.push_back(Node{ TextPtr });

      if (index >= Nodes.size() - 1)
         return newIndex;

      return AddSibling(index, newIndex);
   }

   size_t TextTree::AddSibling(size_t index, size_t newIndex)
   {
      while (Nodes[index].NextSiblingIndex != -1)
         index = Nodes[index].NextSiblingIndex;

      Nodes[index].NextSiblingIndex = newIndex;

      return newIndex;
   }

   size_t TextTree::CountSiblings(size_t index) const
   {
      size_t count = 0;
      while (index < Nodes.size())
      {
         index = Nodes[index].NextSiblingIndex;
         count++;
      }
      return count;
   }

   size_t TextTree::CountChildren(size_t index) const
   {
      if (index >= Nodes.size())
         return 0;

      return CountSiblings(Nodes[index].FirstChildIndex);
   }

   wostream& operator<<(wostream& stream, const TextTree& tree)
   {
      VisitInOrder(tree, 0, [&stream](const TextTree::Node& node, size_t index, size_t level)
      {
         for (size_t indent = 0; indent < level; ++indent)
            stream << L"  ";

         stream << *node.TextPtr << L"\n";

         return true;
      });

      return stream;
   }
}
