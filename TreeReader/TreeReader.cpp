#include "TreeReader.h"

#include <algorithm>

void PrintNode(std::wostream& stream, const TextTree& tree, const TextTree::Node& node, size_t level)
{
   for (size_t indent = 0; indent < level; ++indent)
      stream << L"  ";

   stream << *node.TextPtr << L"\n";

   const size_t endIndex = std::min(node.FirstChildIndex + node.ChildrenCount, tree.Nodes.size());
   for (size_t childIndex = node.FirstChildIndex; childIndex < endIndex; ++childIndex)
      PrintNode(stream, tree, tree.Nodes[childIndex], level + 1);
}

std::wostream& operator<<(std::wostream& stream, const TextTree& tree)
{
   if (tree.Nodes.size() <= 0)
      return stream;

   const size_t endIndex = std::min(tree.Nodes[0].FirstChildIndex, tree.Nodes.size());
   for (size_t i = 0; i < endIndex; ++i)
      PrintNode(stream, tree, tree.Nodes[i], 0);

   return stream;
}


void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const TextFilter& filter)
{
   filteredTree.SourceTextLines = sourceTree.SourceTextLines;
   filteredTree.Nodes.reserve(sourceTree.Nodes.size());

   // Filter the text lines and build a map from source indexes to filtered indexes.
   // If there is no mapping because the text was filtered out, then the index will be -1.
   std::vector<size_t> sourceIndexToFilteredIndex;
   sourceIndexToFilteredIndex.reserve(sourceTree.Nodes.size());

   for (const TextTree::Node& sourceNode : sourceTree.Nodes)
   {
      const Text& text = *sourceNode.TextPtr;
      if (filter.IsKept(text))
      {
         sourceIndexToFilteredIndex.emplace_back(filteredTree.Nodes.size());
         filteredTree.Nodes.emplace_back(TextTree::Node{ &text, size_t(-1), 0 });
      }
      else
      {
         sourceIndexToFilteredIndex.emplace_back(-1);
      }
   }

   for (size_t sourceNodeIndex = 0; sourceNodeIndex < sourceTree.Nodes.size(); ++sourceNodeIndex)
   {
      const size_t filteredNodeIndex = sourceIndexToFilteredIndex[sourceNodeIndex];
      if (filteredNodeIndex == -1)
         continue;

      const TextTree::Node& sourceNode = sourceTree.Nodes[sourceNodeIndex];
      size_t sourceChildIndex = sourceNode.FirstChildIndex;
      if (sourceChildIndex <= 0)
         continue;

      TextTree::Node& filteredNode = filteredTree.Nodes[filteredNodeIndex];
      filteredNode.FirstChildIndex = sourceIndexToFilteredIndex[sourceChildIndex];

      const size_t sourceChildIndexEnd = sourceChildIndex + sourceNode.ChildrenCount;
      for (; sourceChildIndex < sourceChildIndexEnd; ++sourceChildIndex)
      {
         const size_t filteredChildIndex = sourceIndexToFilteredIndex[sourceChildIndex];
         if (filteredChildIndex == -1)
            continue;

         filteredNode.ChildrenCount++;
      }
   }
}

