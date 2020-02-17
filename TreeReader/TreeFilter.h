#pragma once

#include "TextTree.h"

#include <string>
#include <memory>
#include <vector>
#include <regex>

namespace TreeReader
{
   typedef std::wstring Text;

   struct TreeFilter
   {
      virtual ~TreeFilter() {};
      virtual bool IsKept(const TextTree::Node& node, size_t index, size_t level) = 0;
   };

   typedef std::shared_ptr<TreeFilter> TreeFilterPtr;

   struct AcceptTreeFilter : TreeFilter
   {
      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
   };

   struct ContainsTreeFilter : TreeFilter
   {
      const Text Contained;

      ContainsTreeFilter(const Text& text) : Contained(text) { }

      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
   };

   struct RegexTreeFilter : TreeFilter
   {
      const std::wregex Regex;

      RegexTreeFilter(const std::wregex& reg) : Regex(reg) { }

      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
   };

   struct CombineTreeFilter : TreeFilter
   {
      std::vector<TreeFilterPtr> Filters;

      CombineTreeFilter() = default;

      CombineTreeFilter(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) { Filters.push_back(lhs); Filters.push_back(rhs); }
   };

   struct NotTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;

      NotTreeFilter(const TreeFilterPtr& filter) : Filter(filter) { }

      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
   };

   struct OrTreeFilter : CombineTreeFilter
   {
      OrTreeFilter() = default;
      OrTreeFilter(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) : CombineTreeFilter(lhs, rhs) { }

      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
   };

   struct AndTreeFilter : CombineTreeFilter
   {
      AndTreeFilter() = default;
      AndTreeFilter(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) : CombineTreeFilter(lhs, rhs) { }

      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
   };

   struct ApplyUnderTreeFilter : TreeFilter
   {
      TreeFilterPtr Under;
      TreeFilterPtr Filter;
      bool IncludeSelf = false;

      ApplyUnderTreeFilter(const TreeFilterPtr& under, const TreeFilterPtr& filter, bool includeSelf = false)
         : Under(under), Filter(filter), IncludeSelf(includeSelf) {}

      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;

      private:
      size_t _applyOtherFilterUnderLevel = -1;
   };

   struct LevelRangeTreeFilter : TreeFilter
   {
      size_t MinLevel;
      size_t MaxLevel;

      LevelRangeTreeFilter(size_t minLevel, size_t maxLevel) : MinLevel(minLevel), MaxLevel(maxLevel) {}

      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;
   };

   inline std::shared_ptr<AcceptTreeFilter> All() { return std::make_shared<AcceptTreeFilter>(); }
   inline std::shared_ptr<ContainsTreeFilter> Contains(const Text& text) { return std::make_shared<ContainsTreeFilter>(text); }
   inline std::shared_ptr<RegexTreeFilter> Regex(const wchar_t* reg) { return std::make_shared<RegexTreeFilter>(std::wregex(reg)); }
   inline std::shared_ptr<RegexTreeFilter> Regex(const std::wregex& reg) { return std::make_shared<RegexTreeFilter>(reg); }
   inline std::shared_ptr<NotTreeFilter> Not(const TreeFilterPtr& filter) { return std::make_shared<NotTreeFilter>(filter); }
   inline std::shared_ptr<OrTreeFilter> Or(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) { return std::make_shared<OrTreeFilter>(lhs, rhs); }
   inline std::shared_ptr<AndTreeFilter> And(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) { return std::make_shared<AndTreeFilter>(lhs, rhs); }
   inline std::shared_ptr<ApplyUnderTreeFilter> Under(const TreeFilterPtr& under, const TreeFilterPtr& filter, bool includeSelf = false) { return std::make_shared<ApplyUnderTreeFilter>(under, filter, includeSelf); }
   inline std::shared_ptr<ApplyUnderTreeFilter> NoChild(const TreeFilterPtr& filter, bool includeSelf = false) { return Under(filter, Not(All()), includeSelf); }
   inline std::shared_ptr<LevelRangeTreeFilter> LevelRange(size_t min, size_t max) { return std::make_shared<LevelRangeTreeFilter>(min, max); }
   inline std::shared_ptr<LevelRangeTreeFilter> MinLevel(size_t level) { return LevelRange(level, -1); }
   inline std::shared_ptr<LevelRangeTreeFilter> MaxLevel(size_t level) { return LevelRange(0, level); }

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const TreeFilterPtr& filter);
}

