#include "TreeFilter.h"
#include "TextTreeVisitor.h"

namespace TreeReader
{
   bool NotTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return !Filter || !Filter->IsKept(node, index, level);
   }

   bool OrTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      for (const auto& filter : Filters)
         if (filter && filter->IsKept(node, index, level))
            return true;
      return false;
   }

   bool AndTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      for (const auto& filter : Filters)
         if (filter && !filter->IsKept(node, index, level))
            return false;
      return true;
   }

   bool TextTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return !Filter || Filter->IsKept(*node.TextPtr);
   }

   bool RemoveChildrenTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter)
         return true;

      if (level > _removeUnderLevel)
         return false;

      if (level <= _removeUnderLevel)
         _removeUnderLevel = -1;

      const bool kept = Filter->IsKept(node, index, level);
      if (kept)
         return true;

      _removeUnderLevel = level;
      return kept;
   }

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const std::shared_ptr<TextFilter>& filter)
   {
      FilterTree(sourceTree, filteredTree, std::make_shared<TextTreeFilter>(filter));
   }

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const std::shared_ptr<TreeFilter>& filter)
   {
      if (!filter)
      {
         filteredTree = sourceTree;
         return;
      }

      filteredTree.SourceTextLines = sourceTree.SourceTextLines;
      filteredTree.Nodes.reserve(sourceTree.Nodes.size());

      // This keeps the current branch of nodes we have created.
      // We will keep one entry per source level, even when some
      // levels were filtered out.
      //
      std::vector<size_t> filteredBranchIndexes;
      std::vector<bool> fillChildren;

      // To make the algorithm work the same way for the first node
      // we pretend that we've seen a preceeding sibling of the level
      // zero and did not keep it.
      //
      // So the current filtered branch for level zero is zero.
      filteredBranchIndexes.push_back(0);
      fillChildren.push_back(false);

      VisitInOrder(sourceTree, 0, [&](const TextTree::Node& sourceNode, const size_t sourceIndex, const size_t sourceLevel)
      {
         filteredBranchIndexes.resize(sourceLevel + 1, -1);
         fillChildren.resize(sourceLevel + 1, false);

         // Either the index of the newly created filtered node if kept, or -1 if not kept.
         size_t filteredIndex = -1;

         if (filter->IsKept(sourceNode, sourceIndex, sourceLevel))
         {
            // Connect to the nearest node in the branch.
            for (size_t level = sourceLevel; level < filteredBranchIndexes.size(); --level)
            {
               if (filteredBranchIndexes[level] != -1)
               {
                  // If the node is at the same level, do not add as a child.
                  if (level < sourceLevel && fillChildren[level])
                  {
                     filteredIndex = filteredTree.AddChild(filteredBranchIndexes[level], sourceNode.TextPtr);
                  }
                  else
                  {
                     filteredIndex = filteredTree.AddSibling(filteredBranchIndexes[level], sourceNode.TextPtr);
                  }
                  break;
               }
            }
         }

         // If kept, this node is the new active node for this level.
         // If not kept, do not over-write a sibling node that may exists at this level.
         filteredBranchIndexes.resize(sourceLevel + 1, -1);
         if (filteredIndex != -1)
            filteredBranchIndexes[sourceLevel] = filteredIndex;

         // If the node is kept, start to add sub-node as children.
         // If not kept, make any existing singling node begin to add node as sibling instead
         // of children.
         fillChildren.resize(sourceLevel + 1, false);
         fillChildren[sourceLevel] = (filteredIndex != -1);

         return true;
      });
   }
}
