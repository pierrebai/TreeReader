#include "TreeFiltering.h"
#include "TreeFilter.h"

namespace TreeReader
{
   using namespace std;
   using Result = TreeFilter::Result;
   using Node = TextTree::Node;

   FilterTreeVisitor::FilterTreeVisitor(const TextTree& sourceTree, TextTree& filteredTree, TreeFilter& filter)
      : FilteredTree(filteredTree), Filter(filter)
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

      const TreeFilter::Result result = Filter.IsKept(tree, sourceNode, sourceLevel);
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

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, TreeFilter& filter)
   {
      FilterTreeVisitor visitor(sourceTree, filteredTree, filter);
      VisitInOrder(sourceTree, visitor);
   }

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const TreeFilterPtr& filter)
   {
      if (!filter)
      {
         filteredTree = sourceTree;
         return;
      }

      FilterTree(sourceTree, filteredTree, *filter);
   }

   AsyncFilterTreeResult FilterTreeAsync(const shared_ptr<TextTree>& sourceTree, const TreeFilterPtr& filter)
   {
      if (!filter)
         return {};

      auto abort = make_shared<CanAbortTreeVisitor>();
      auto fut = async(launch::async, [sourceTree, filter, abort]()
      {
         TextTree filtered;
         abort->Visitor = make_shared<FilterTreeVisitor>(*sourceTree, filtered, *filter);
         VisitInOrder(*sourceTree, *abort);
         return filtered;
      });

      return make_pair(move(fut), abort);
   }
}
