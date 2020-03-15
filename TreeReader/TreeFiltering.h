#pragma once

#include "TextTreeVisitor.h"

#include <memory>
#include <vector>
#include <future>

namespace TreeReader
{
   struct TreeFilter;
   typedef std::shared_ptr<TreeFilter> TreeFilterPtr;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Filters a source tree into a filtered tree using the given filter.

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const TreeFilterPtr& filter);

   using AsyncFilterTreeResult = std::pair<std::future<TextTree>, std::shared_ptr<CanAbortTreeVisitor>>;

   AsyncFilterTreeResult FilterTreeAsync(const std::shared_ptr<TextTree>& sourceTree, const TreeFilterPtr& filter);

   ////////////////////////////////////////////////////////////////////////////
   //
   // The tree visitor that actually does the filtering.

   struct FilterTreeVisitor : SimpleTreeVisitor
   {
      TextTree& FilteredTree;
      TreeFilterPtr Filter;

      FilterTreeVisitor(const TextTree& sourceTree, TextTree& filteredTree, const TreeFilterPtr& filter);

      Result Visit(const TextTree& tree, const TextTree::Node& sourceNode, const size_t sourceLevel) override;

   private:
      // This keeps the current branch of nodes we have created.
      // We will keep one entry per source level, even when some
      // levels were filtered out.
      std::vector<TextTree::Node*> _filteredBranchNodes;
      std::vector<bool> _fillChildren;
   };

}

