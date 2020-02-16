#pragma once

#include "TextTree.h"
#include "TextFilter.h"

struct TreeFilter
{
   virtual bool IsKept(const TextTree::Node& node, size_t index, size_t level) = 0;
};

struct CombineTreeFilter : TreeFilter
{
   std::vector<std::shared_ptr<TreeFilter>> Filters;

   CombineTreeFilter() = default;

   CombineTreeFilter(const std::shared_ptr<TreeFilter>& lhs, const std::shared_ptr<TreeFilter>& rhs) { Filters.push_back(lhs); Filters.push_back(rhs); }
};

struct NotTreeFilter : TreeFilter
{
   std::shared_ptr<TreeFilter> Filter;

   NotTreeFilter(const std::shared_ptr<TreeFilter>& filter) : Filter(filter) { }

   bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
};

struct OrTreeFilter : CombineTreeFilter
{
   OrTreeFilter() = default;
   OrTreeFilter(const std::shared_ptr<TreeFilter>& lhs, const std::shared_ptr<TreeFilter>& rhs) : CombineTreeFilter(lhs, rhs) { }

   bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
};

struct AndTreeFilter : CombineTreeFilter
{
   AndTreeFilter() = default;
   AndTreeFilter(const std::shared_ptr<TreeFilter>& lhs, const std::shared_ptr<TreeFilter>& rhs) : CombineTreeFilter(lhs, rhs) { }

   bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
};

struct TextTreeFilter : TreeFilter
{
   std::shared_ptr<TextFilter> Filter;

   TextTreeFilter(const std::shared_ptr<TextFilter>& filter) : Filter(filter) {}

   bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
};

struct RemoveChildrenTreeFilter : TreeFilter
{
   std::shared_ptr<TreeFilter> Filter;

   RemoveChildrenTreeFilter(const std::shared_ptr<TreeFilter>& filter) : Filter(filter) {}

   bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;

private:
   size_t _removeUnderLevel = -1;
};

inline std::shared_ptr<TextTreeFilter> ContainsText(const Text& text) { return std::make_shared<TextTreeFilter>(std::make_shared<ContainsTextFilter>(text)); }
inline std::shared_ptr<NotTreeFilter> Not(const std::shared_ptr<TreeFilter>& filter) { return std::make_shared<NotTreeFilter>(filter); }
inline std::shared_ptr<OrTreeFilter> Or(const std::shared_ptr<TreeFilter>& lhs, const std::shared_ptr<TreeFilter>& rhs) { return std::make_shared<OrTreeFilter>(lhs, rhs); }
inline std::shared_ptr<AndTreeFilter> And(const std::shared_ptr<TreeFilter>& lhs, const std::shared_ptr<TreeFilter>& rhs) { return std::make_shared<AndTreeFilter>(lhs, rhs); }
inline std::shared_ptr<RemoveChildrenTreeFilter> NoChild(const std::shared_ptr<TreeFilter>& filter) { return std::make_shared<RemoveChildrenTreeFilter>(filter); }

void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const std::shared_ptr<TextFilter>& filter);
void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const std::shared_ptr<TreeFilter>& filter);

