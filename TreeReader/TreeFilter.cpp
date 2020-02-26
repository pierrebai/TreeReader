#include "TreeFilter.h"
#include "TextTreeVisitor.h"

namespace TreeReader
{
   using namespace std;
   using Result = TreeFilter::Result;
   using Node = TextTree::Node;

   constexpr Result Keep{ false, false, true };
   constexpr Result Drop{ false, false, false };
   constexpr Result StopAndKeep{ true, false, true };
   constexpr Result StopAndDrop{ true, false, false };
   constexpr Result DropAndSkip{ false, true, false };
   constexpr Result KeepAndSkip{ false, true, true };

   Result AcceptTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return Keep;
   }

   Result StopTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return Keep ? StopAndKeep : StopAndDrop;
   }

   Result UntilTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (!Filter)
         return StopAndDrop;

      return Filter->IsKept(tree, node, level).Keep ? StopAndDrop : Drop;
   }

   Result ContainsTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return (wcsstr(node.TextPtr, Contained.c_str()) != nullptr) ? Keep : Drop;
   }

   Result TextAddressTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return (ExactAddress == node.TextPtr) ? Keep : Drop;
   }

   Result RegexTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return regex_search(node.TextPtr, Regex) ? Keep : Drop;
   }

   Result NotTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      Result result = Filter->IsKept(tree, node, level);
      result.Keep = !result.Keep;
      return result;
   }

   Result OrTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      Result result = Drop;
      for (const auto& filter : Filters)
         if (filter)
            if (result = result | filter->IsKept(tree, node, level); result.Keep)
               break;
      return result;
   }

   Result AndTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      Result result = Keep;
      for (const auto& filter : Filters)
         if (filter)
            if (result = result & filter->IsKept(tree, node, level); !result.Keep)
               break;
      return result;
   }

   Result UnderTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      // If we have found a match previously for the under filter,
      // then keep the nodes while we're still in levels deeper
      // than where we found the match.
      if (level > _keepAllNodesUnderLevel)
         return Keep;

      // If we've reached back up to the level where we found the match previously,
      // then stop keeping nodes. We do this by making the apply under level very large.
      if (level <= _keepAllNodesUnderLevel)
         _keepAllNodesUnderLevel = -1;

      // If the node doesn't match the under filter, don't apply the other filter.
      // Just return the result of the other filter.
      Result result = Filter->IsKept(tree, node, level);
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

   Result CountSiblingsTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      // If we've reached back up higher than the level where we found the match previously,
      // then stop keeping nodes. We do this by making the apply under level very large
      // and the count to zero.
      if (level < _keepNodesAtLevel)
      {
         _keepNodesAtLevel = -1;
         _countdown = 0;
      }

      // If we previously found a match and there are still a number of nodes to be accepted,
      // reduce that number and keep the node.
      if (_countdown > 0 && level == _keepNodesAtLevel)
      {
         --_countdown;
         return Keep;
      }

      // If the node doesn't match the other filter, don't apply the other filter.
      // Just return the result of the other filter.
      Result result = Filter->IsKept(tree, node, level);
      if (!result.Keep)
         return result;

      // If we reach here, the other filter matched, so we will start accepting
      // a number of nodes under this one.
      //
      // Record the level at which we must come back up to to stop accepting nodes
      // without checking the filter.
      _keepNodesAtLevel = level;
      _countdown = Count;

      // If the filter must not keep the matching node, then set Keep to false here.
      // apply it here.
      if (!IncludeSelf)
         result.Keep = false;

      return result;
   }

   Result CountChildrenTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      // If we've reached back up to the level where we found the match previously,
      // then stop keeping nodes. We do this by making the apply under level very large
      // and the count to zero.
      if (level <= _keepNodesUnderLevel)
      {
         _keepNodesUnderLevel = -1;
         _countdown = 0;
      }

      // If we previously found a match and there are still a number of nodes to be accepted,
      // reduce that number and keep the node.
      if (_countdown > 0 && level > _keepNodesUnderLevel)
      {
         --_countdown;
         return Keep;
      }

      // If the node doesn't match the other filter, don't apply the other filter.
      // Just return the result of the other filter.
      Result result = Filter->IsKept(tree, node, level);
      if (!result.Keep)
         return result;

      // If we reach here, the other filter matched, so we will start accepting
      // a number of nodes under this one.
      //
      // Record the level at which we must come back up to to stop accepting nodes
      // without checking the filter.
      _keepNodesUnderLevel = level;
      _countdown = Count;

      // If the filter must not keep the matching node, then set Keep to false here.
      // apply it here.
      if (!IncludeSelf)
         result.Keep = false;

      return result;
   }

   Result RemoveChildrenTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      if (!Filter->IsKept(tree, node, level).Keep)
         return Keep;

      return RemoveSelf ? DropAndSkip : KeepAndSkip;
   }

   Result LevelRangeTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (level < MinLevel)
         return Drop;

      if (level <= MaxLevel)
         return Keep;

      return DropAndSkip;
   }

   Result IfSubTreeTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      FilterTreeVisitor visitor(tree, _filtered, Filter);
      VisitInOrder(tree, &node, false, visitor);
      return (_filtered.Roots.size() > 0) ? Keep : Drop;
   }

   Result IfSiblingTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      const auto& children = node.Parent ? node.Parent->Children : tree.Roots;
      auto pos = children.begin();
      pos += node.IndexInParent;

      for (; pos != children.end(); ++pos)
      {
         const Node& node = **pos;
         const auto result = Filter->IsKept(tree, node, level);
         if (result.Keep)
            return Keep;
         if (result.Stop)
            break;
      }

      return Drop;
   }

   FilterTreeVisitor::FilterTreeVisitor(const TextTree& sourceTree, TextTree& filteredTree, const TreeFilterPtr& filter)
   : FilteredTree(filteredTree), Filter (filter)
   {
      filteredTree.Reset();
      filteredTree.SourceTextLines = sourceTree.SourceTextLines;

      // To make the algorithm work the same way for the first node
      // we pretend that we've seen a preceeding sibling of the level
      // zero and did not keep it.
      //
      // So the current filtered branch for level zero is null.
      _filteredBranchNodes.push_back(nullptr);
      _fillChildren.push_back(false);
   }

   TreeVisitor::Result FilterTreeVisitor::Visit(const TextTree& tree, const Node& sourceNode, const size_t sourceLevel)
   {
      _filteredBranchNodes.resize(sourceLevel + 1, nullptr);
      _fillChildren.resize(sourceLevel + 1, false);

      // Either the index of the newly created filtered node if kept, or -1 if not kept.
      Node* filteredNode = nullptr;

      const TreeFilter::Result result = Filter->IsKept(tree, sourceNode, sourceLevel);
      if (result.Keep)
      {
         // Connect to the nearest node in the branch.
         Node* addUnder = nullptr;
         for (size_t level = sourceLevel; level < _filteredBranchNodes.size(); --level)
         {
            if (_filteredBranchNodes[level])
            {
               // If the node is at the same level, do not add as a child.
               addUnder = (level < sourceLevel && _fillChildren[level]) ? _filteredBranchNodes[level] : _filteredBranchNodes[level]->Parent;
               break;
            }
         }

         filteredNode = FilteredTree.AddChild(addUnder, sourceNode.TextPtr);
      }

      // If kept, this node is the new active node for this level.
      // If not kept, do not over-write a sibling node that may exists at this level.
      _filteredBranchNodes.resize(sourceLevel + 1, nullptr);
      if (filteredNode)
         _filteredBranchNodes[sourceLevel] = filteredNode;

      // If the node is kept, start to add sub-node as children.
      // If not kept, make any existing singling node begin to add node as sibling instead
      // of children.
      _fillChildren.resize(sourceLevel + 1, false);
      _fillChildren[sourceLevel] = (filteredNode != nullptr);

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
