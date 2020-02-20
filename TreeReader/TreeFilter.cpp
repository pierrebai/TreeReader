#include "TreeFilter.h"
#include "TextTreeVisitor.h"

namespace TreeReader
{
   using namespace std;
   using Result = TreeFilter::Result;
   constexpr Result Keep{ false, false, true };
   constexpr Result Drop{ false, false, false };
   constexpr Result StopAndKeep{ true, false, true };
   constexpr Result StopAndDrop{ true, false, false };
   constexpr Result DropAndSkip{ false, true, false };
   constexpr Result KeepAndSkip{ false, true, true };

   Result AcceptTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      return Keep;
   }

   Result StopTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      return Keep ? StopAndKeep : StopAndDrop;
   }

   Result UntilTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter)
         return StopAndDrop;

      return Filter->IsKept(tree, node, index, level).Keep ? StopAndDrop : Drop;
   }

   Result ContainsTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      return (node.TextPtr->find(Contained) != wstring::npos) ? Keep : Drop;
   }

   Result RegexTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      return regex_search(*node.TextPtr, Regex) ? Keep : Drop;
   }

   Result NotTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter)
         return Keep;

      Result result = Filter->IsKept(tree, node, index, level);
      result.Keep = !result.Keep;
      return result;
   }

   Result OrTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      Result result = Drop;
      for (const auto& filter : Filters)
         if (filter)
            if (result = result | filter->IsKept(tree, node, index, level); result.Keep)
               break;
      return result;
   }

   Result AndTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      Result result = Keep;
      for (const auto& filter : Filters)
         if (filter)
            if (result = result & filter->IsKept(tree, node, index, level); !result.Keep)
               break;
      return result;
   }

   Result UnderTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
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
      Result result = Filter->IsKept(tree, node, index, level);
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

   Result RemoveChildrenTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter)
         return Keep;

      if (!Filter->IsKept(tree, node, index, level).Keep)
         return Keep;

      return RemoveSelf ? DropAndSkip : KeepAndSkip;
   }

   Result LevelRangeTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      if (level < MinLevel)
         return Drop;

      if (level <= MaxLevel)
         return Keep;

      return DropAndSkip;
   }

   Result IfSubTreeTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t index, size_t level)
   {
      if (!Filter)
         return Keep;

      FilterTreeVisitor visitor(tree, _filtered, Filter);
      VisitInOrder(tree, index, false, visitor);
      return (_filtered.Nodes.size() > 0) ? Keep : Drop;
   }

   Result IfSiblingTreeFilter::IsKept(const TextTree& tree, const TextTree::Node&, size_t index, size_t level)
   {
      if (!Filter)
         return Keep;

      while (index < tree.Nodes.size())
      {
         const TextTree::Node& node = tree.Nodes[index];
         const auto result = Filter->IsKept(tree, node, index, level);
         if (result.Keep)
            return Keep;
         if (result.Stop)
            break;
         index = node.NextSiblingIndex;
      }

      return Drop;
   }

   FilterTreeVisitor::FilterTreeVisitor(const TextTree& sourceTree, TextTree& filteredTree, const TreeFilterPtr& filter)
   : FilteredTree(filteredTree), Filter (filter)
   {
      filteredTree.Nodes.clear();
      filteredTree.SourceTextLines = sourceTree.SourceTextLines;
      filteredTree.Nodes.reserve(sourceTree.Nodes.size());

      // To make the algorithm work the same way for the first node
      // we pretend that we've seen a preceeding sibling of the level
      // zero and did not keep it.
      //
      // So the current filtered branch for level zero is zero.
      _filteredBranchIndexes.push_back(0);
      _fillChildren.push_back(false);
   }

   TreeVisitor::Result FilterTreeVisitor::Visit(const TextTree& tree, const TextTree::Node& sourceNode, const size_t sourceIndex, const size_t sourceLevel)
   {
      _filteredBranchIndexes.resize(sourceLevel + 1, -1);
      _fillChildren.resize(sourceLevel + 1, false);

      // Either the index of the newly created filtered node if kept, or -1 if not kept.
      size_t filteredIndex = -1;

      const TreeFilter::Result result = Filter->IsKept(tree, sourceNode, sourceIndex, sourceLevel);
      if (result.Keep)
      {
         // Connect to the nearest node in the branch.
         for (size_t level = sourceLevel; level < _filteredBranchIndexes.size(); --level)
         {
            if (_filteredBranchIndexes[level] != -1)
            {
               // If the node is at the same level, do not add as a child.
               if (level < sourceLevel && _fillChildren[level])
               {
                  filteredIndex = FilteredTree.AddChild(_filteredBranchIndexes[level], sourceNode.TextPtr);
               }
               else
               {
                  filteredIndex = FilteredTree.AddSibling(_filteredBranchIndexes[level], sourceNode.TextPtr);
               }
               break;
            }
         }
      }

      // If kept, this node is the new active node for this level.
      // If not kept, do not over-write a sibling node that may exists at this level.
      _filteredBranchIndexes.resize(sourceLevel + 1, -1);
      if (filteredIndex != -1)
         _filteredBranchIndexes[sourceLevel] = filteredIndex;

      // If the node is kept, start to add sub-node as children.
      // If not kept, make any existing singling node begin to add node as sibling instead
      // of children.
      _fillChildren.resize(sourceLevel + 1, false);
      _fillChildren[sourceLevel] = (filteredIndex != -1);

      // Note: we really do want to slice the result down to the TreeVisitor::Result type.
      return TreeVisitor::Result(result);
   }

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const shared_ptr<TreeFilter>& filter)
   {
      if (!filter)
      {
         filteredTree = sourceTree;
         return;
      }

      FilterTreeVisitor visitor(sourceTree, filteredTree, filter);
      VisitInOrder(sourceTree, 0, visitor);
   }
}
