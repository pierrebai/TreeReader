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
      virtual bool IsKept(const TextTree::Node& node, size_t index, size_t level) = 0;
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

   struct RemoveChildrenTreeFilter : TreeFilter
   {
      std::shared_ptr<TreeFilter> Filter;

      RemoveChildrenTreeFilter(const std::shared_ptr<TreeFilter>& filter) : Filter(filter) {}

      bool IsKept(const TextTree::Node& node, size_t index, size_t level) override;

   private:
      size_t _removeUnderLevel = -1;
   };

   inline std::shared_ptr<ContainsTreeFilter> Contains(const Text& text) { return std::make_shared<ContainsTreeFilter>(text); }
   inline std::shared_ptr<RegexTreeFilter> Regex(const wchar_t* reg) { return std::make_shared<RegexTreeFilter>(std::wregex(reg)); }
   inline std::shared_ptr<RegexTreeFilter> Regex(const std::wregex& reg) { return std::make_shared<RegexTreeFilter>(reg); }
   inline std::shared_ptr<NotTreeFilter> Not(const std::shared_ptr<TreeFilter>& filter) { return std::make_shared<NotTreeFilter>(filter); }
   inline std::shared_ptr<OrTreeFilter> Or(const std::shared_ptr<TreeFilter>& lhs, const std::shared_ptr<TreeFilter>& rhs) { return std::make_shared<OrTreeFilter>(lhs, rhs); }
   inline std::shared_ptr<AndTreeFilter> And(const std::shared_ptr<TreeFilter>& lhs, const std::shared_ptr<TreeFilter>& rhs) { return std::make_shared<AndTreeFilter>(lhs, rhs); }
   inline std::shared_ptr<RemoveChildrenTreeFilter> NoChild(const std::shared_ptr<TreeFilter>& filter) { return std::make_shared<RemoveChildrenTreeFilter>(filter); }

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const std::shared_ptr<TreeFilter>& filter);
}

