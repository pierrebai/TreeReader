#include "TextTree.h"
#include "TextTreeVisitor.h"

namespace TreeReader
{
   using namespace std;

   size_t TextTree::AddChild(size_t underIndex, const wchar_t* text)
   {
      const size_t newIndex = Nodes.size();
      Nodes.push_back(Node{ text, underIndex });

      if (underIndex >= Nodes.size() - 1)
      {
         Nodes[newIndex].ChildInParent = RootCount;
         RootCount += 1;
         return newIndex;
      }

      return AddChild(underIndex, newIndex);
   }

   size_t TextTree::AddChild(size_t underIndex, size_t newIndex)
   {
      Nodes[newIndex].ChildInParent = Nodes[underIndex].ChildrenCount;
      Nodes[underIndex].ChildrenCount += 1;

      if (Nodes[underIndex].FirstChildIndex == -1)
      {
         Nodes[underIndex].FirstChildIndex = newIndex;
         return newIndex;
      }
      else
      {
         return AddSibling(Nodes[underIndex].FirstChildIndex, newIndex);
      }
   }

   size_t TextTree::AddSibling(size_t afterIndex, const wchar_t* text)
   {
      const size_t newIndex = Nodes.size();
      Nodes.push_back(Node{ text });
      Node& newNode = Nodes.back();

      if (afterIndex >= Nodes.size() - 1)
      {
         newNode.ChildInParent = RootCount;
         RootCount += 1;
         return newIndex;
      }

      const size_t parentIndex = Nodes[afterIndex].ParentIndex;
      newNode.ParentIndex = parentIndex;

      if (parentIndex < Nodes.size())
      {
         newNode.ChildInParent = Nodes[afterIndex].ChildrenCount;
         Nodes[parentIndex].ChildrenCount += 1;
      }
      else
      {
         newNode.ChildInParent = RootCount;
         RootCount += 1;
      }

      return AddSibling(afterIndex, newIndex);
   }

   size_t TextTree::AddSibling(size_t afterIndex, size_t newIndex)
   {
      while (Nodes[afterIndex].NextSiblingIndex != -1)
         afterIndex = Nodes[afterIndex].NextSiblingIndex;

      Nodes[afterIndex].NextSiblingIndex = newIndex;

      return newIndex;
   }

   size_t TextTree::CountSiblings(size_t fromIndex) const
   {
      if (fromIndex >= Nodes.size())
         return 0;

      const size_t parentIndex = Nodes[fromIndex].ParentIndex;
      if (parentIndex >= Nodes.size())
         return RootCount;

      return Nodes[parentIndex].ChildrenCount;
   }

   size_t TextTree::CountChildren(size_t parentIndex) const
   {
      if (parentIndex >= Nodes.size())
         return RootCount;

      return Nodes[parentIndex].ChildrenCount;
   }

   size_t TextTree::CountAncestors(size_t fromIndex) const
   {
      size_t count = 0;

      while (Nodes[fromIndex].ParentIndex < Nodes.size())
      {
         count += 1;
         fromIndex = Nodes[fromIndex].ParentIndex;
      }

      return count;
   }

   std::wostream& PrintTree(std::wostream& stream, const TextTree& tree, const std::wstring& indentation)
   {
      VisitInOrder(tree, 0, [&stream, &indentation](const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
      {
         for (size_t indent = 0; indent < level; ++indent)
            stream << indentation;

         stream << node.TextPtr << L"\n";

         return TreeVisitor::Result();
      });
      return stream;
   }

   wostream& operator<<(wostream& stream, const TextTree& tree)
   {
      return PrintTree(stream, tree);
   }
}
