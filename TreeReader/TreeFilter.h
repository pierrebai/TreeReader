#pragma once

#include "TextTree.h"
#include "TextFilter.h"

struct TreeFilter
{
   virtual bool IsKept(const TextTree::Node& node, size_t index, size_t level) = 0;
};

struct TextTreeFilter : TreeFilter
{
   TextFilter& Filter;

   TextTreeFilter(TextFilter& filter) : Filter(filter) {}

   bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
};

struct RemoveChildrenTreeFilter : TreeFilter
{
   TreeFilter& Filter;

   RemoveChildrenTreeFilter(TreeFilter& filter) : Filter(filter) {}

   bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;

private:
   size_t _removeUnderLevel = -1;
};

struct MultiTreeFilters : TreeFilter
{
   std::vector<std::shared_ptr<TreeFilter>> Filters;
   size_t RootIndex = 0;

   bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
};

void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, TreeFilter& filter);
void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, TextFilter& filter);

