#include "TreeFilter.h"
#include "TextTreeVisitor.h"

namespace TreeReader
{
   using namespace std;

   bool AcceptTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return true;
   }

   bool ContainsTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return node.TextPtr->find(Contained) != wstring::npos;
   }

   bool RegexTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return regex_search(*node.TextPtr, Regex);
   }

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

   bool ApplyUnderTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter || !Under)
         return true;

      // If we have found a match previously for the under filter,
      // then apply the other filter while we're still in levels deeper
      // than where we found the match.
      if (level > _applyOtherFilterUnderLevel)
         return Filter->IsKept(node, index, level);

      // If we've reach back up to the level where we found the match previously,
      // then stop applying the olther filter.
      if (level <= _applyOtherFilterUnderLevel)
         _applyOtherFilterUnderLevel = -1;

      // If the node doesn't match the under filter, don't apply the other filter.
      // Just accept the node.
      const bool kept = Under->IsKept(node, index, level);
      if (!kept)
         return true;

      // If we reach here, the under filter matched, so we will apply the other filter
      // to nodes under this one.
      //
      // Record the level at which we must come back up to to stop applying the other filter.
      _applyOtherFilterUnderLevel = level;

      // If the other filter must be applied to the node matching the under filter,
      // apply it here.
      if (IncludeSelf)
         return Filter->IsKept(node, index, level);

      return kept;

   }

   bool LevelRangeTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return level >= MinLevel && level <= MaxLevel;
   }

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const shared_ptr<TreeFilter>& filter)
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
      vector<size_t> filteredBranchIndexes;
      vector<bool> fillChildren;

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
