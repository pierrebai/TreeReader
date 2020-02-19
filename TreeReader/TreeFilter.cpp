#include "TreeFilter.h"
#include "TextTreeVisitor.h"

namespace TreeReader
{
   using namespace std;
   using Result = TreeFilter::Result;
   constexpr Result Keep{ false, false, true };
   constexpr Result Drop{ false, false, false };
   constexpr Result Stop{ true, false, false };
   constexpr Result DropAndSkip{ false, true, false };
   constexpr Result KeepAndSkip{ false, true, true };

   Result AcceptTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return Keep;
   }

   Result ContainsTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return (node.TextPtr->find(Contained) != wstring::npos) ? Keep : Drop;
   }

   Result RegexTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      return regex_search(*node.TextPtr, Regex) ? Keep : Drop;
   }

   Result NotTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter)
         return Keep;

      Result result = Filter->IsKept(node, index, level);
      result.Keep = !result.Keep;
      return result;
   }

   Result OrTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      Result result = Drop;
      for (const auto& filter : Filters)
         if (filter)
            result = result | filter->IsKept(node, index, level);
      return result;
   }

   Result AndTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      Result result = Keep;
      for (const auto& filter : Filters)
         if (filter)
            result = result & filter->IsKept(node, index, level);
      return result;
   }

   Result UnderTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter)
         return Keep;

      // If we have found a match previously for the under filter,
      // then keep the nodes while we're still in levels deeper
      // than where we found the match.
      if (level > _keepAllNodesUnderLevel)
         return Keep;

      // If we've reach back up to the level where we found the match previously,
      // then stop keeping nodes. We do this by making the apply under level very large.
      if (level <= _keepAllNodesUnderLevel)
         _keepAllNodesUnderLevel = -1;

      // If the node doesn't match the under filter, don't apply the other filter.
      // Just accept the node.
      Result result = Filter->IsKept(node, index, level);
      if (!result.Keep)
         return result;

      // If we reach here, the under filter matched, so we will start accepting
      // the nodes under this one.
      //
      // Record the level at which we must come back up to to stop accepting nodes
      // without checking the filter.
      _keepAllNodesUnderLevel = level;

      // If the filter must not keep the matching node, then set Keep to false here.
      // apply it here.
      if (!IncludeSelf)
         result.Keep = false;

      return result;
   }

   Result RemoveChildrenTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter)
         return Keep;

      if (!Filter->IsKept(node, index, level).Keep)
         return Keep;

      return RemoveSelf ? DropAndSkip : KeepAndSkip;
   }

   Result LevelRangeTreeFilter::IsKept(const TextTree::Node& node, size_t index, size_t level)
   {
      if (level < MinLevel)
         return Drop;

      if (level <= MaxLevel)
         return Keep;

      return DropAndSkip;
   }

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const shared_ptr<TreeFilter>& filter)
   {
      if (!filter)
      {
         filteredTree = sourceTree;
         return;
      }

      filteredTree.Nodes.clear();
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

         const Result result = filter->IsKept(sourceNode, sourceIndex, sourceLevel);
         if (result.Keep)
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

         return result;
      });
   }
}
