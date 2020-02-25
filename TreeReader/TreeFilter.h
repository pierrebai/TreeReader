#pragma once

#include "TextTree.h"
#include "TextTreeVisitor.h"

#include <string>
#include <memory>
#include <vector>
#include <regex>

namespace TreeReader
{
   // Filter used to reduce a text tree to another simpler text tree.

   struct TreeFilter
   {
      // Possible result of a filter on a node: stop or not, visit children or not, keep the node or not.

      struct Result : TreeVisitor::Result
      {
         bool Keep = false;

         // Combining results preserves the stop and skip children flags, but combines the keep flags as specified: or, and.

         Result operator|(const Result& r) const
         {
            return Result{ Stop || r.Stop, SkipChildren || r.SkipChildren, Keep || r.Keep };
         }

         Result operator&(const Result& r) const
         {
            return Result{ Stop || r.Stop, SkipChildren || r.SkipChildren, Keep && r.Keep };
         }
      };

      virtual ~TreeFilter() {};

      // The filter of a node.
      virtual Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) = 0;
   };

   typedef std::shared_ptr<TreeFilter> TreeFilterPtr;

   // Filter that accepts all nodes.

   struct AcceptTreeFilter : TreeFilter
   {
      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that stops filtering.
   //
   // Can keep or not keep the node, as desired.

   struct StopTreeFilter : TreeFilter
   {
      bool Keep = true;

      StopTreeFilter() = default;
      StopTreeFilter(bool keep) : Keep(keep) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that stops filtering when another sub-filter keeps a node.
   //
   // Never keeps the node. Used to stop doing sb-tree filtering. (See IfSubTree and IfSibling.)

   struct UntilTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;

      UntilTreeFilter() = default;
      UntilTreeFilter(const TreeFilterPtr& filter) : Filter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that keeps nodes containing a given text.

   struct ContainsTreeFilter : TreeFilter
   {
      std::wstring Contained;

      ContainsTreeFilter() = default;
      ContainsTreeFilter(const std::wstring& text) : Contained(text) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that keeps nodes matching a regular expression.

   struct RegexTreeFilter : TreeFilter
   {
      std::wstring RegexTextForm;
      std::wregex Regex;

      RegexTreeFilter() = default;
      RegexTreeFilter(const std::wstring& reg) : RegexTextForm(reg), Regex(std::wregex(reg)) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that combines the result of other filters.

   struct CombineTreeFilter : TreeFilter
   {
      std::vector<TreeFilterPtr> Filters;

      CombineTreeFilter() = default;

      CombineTreeFilter(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) { Filters.push_back(lhs); Filters.push_back(rhs); }
      CombineTreeFilter(const std::vector<TreeFilterPtr>& filters) : Filters(filters) {}
   };

   // Filter that inverts the keep decision of another filter.

   struct NotTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;

      NotTreeFilter() = default;
      NotTreeFilter(const TreeFilterPtr& filter) : Filter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that accepts a node if any of its sub-filters accept the node.

   struct OrTreeFilter : CombineTreeFilter
   {
      OrTreeFilter() = default;
      OrTreeFilter(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) : CombineTreeFilter(lhs, rhs) { }
      OrTreeFilter(const std::vector<TreeFilterPtr>& filters) : CombineTreeFilter(filters) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that accepts a node if all of its sub-filters accept the node.

   struct AndTreeFilter : CombineTreeFilter
   {
      AndTreeFilter() = default;
      AndTreeFilter(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) : CombineTreeFilter(lhs, rhs) { }
      AndTreeFilter(const std::vector<TreeFilterPtr>& filters) : CombineTreeFilter(filters) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that accepts all children of a node that was accepted by another filter.
   //
   // Can accept or not that parent initial node.

   struct UnderTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;
      bool IncludeSelf = true;

      UnderTreeFilter() = default;
      UnderTreeFilter(const TreeFilterPtr& filter, bool includeSelf = true)
         : Filter(filter), IncludeSelf(includeSelf) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;

      private:
         size_t _keepAllNodesUnderLevel = -1;
   };

   // Filter that removes all children of a node that was accepted by another filter.
   //
   // Can remove or not that parent initial node.

   struct RemoveChildrenTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;
      bool RemoveSelf = false;

      RemoveChildrenTreeFilter() = default;
      RemoveChildrenTreeFilter(const TreeFilterPtr& filter, bool removeSelf) : Filter(filter), RemoveSelf(removeSelf) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that accepts nodes that are within a range of depth in the tree.

   struct LevelRangeTreeFilter : TreeFilter
   {
      size_t MinLevel;
      size_t MaxLevel;

      LevelRangeTreeFilter() = default;
      LevelRangeTreeFilter(size_t minLevel, size_t maxLevel) : MinLevel(minLevel), MaxLevel(maxLevel) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that accepts a node if at least one child is accepted by another filter.

   struct IfSubTreeTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;

      IfSubTreeTreeFilter() = default;
      IfSubTreeTreeFilter(const TreeFilterPtr& filter) : Filter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;

   private:
      TextTree _filtered;
   };

   // Filter that accepts a node if at least one sibling is accepted by another filter.

   struct IfSiblingTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;

      IfSiblingTreeFilter() = default;
      IfSiblingTreeFilter(const TreeFilterPtr& filter) : Filter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Functions to create filters.

   inline std::shared_ptr<AcceptTreeFilter> Accept() { return std::make_shared<AcceptTreeFilter>(); }
   inline std::shared_ptr<StopTreeFilter> Stop() { return std::make_shared<StopTreeFilter>(); }
   inline std::shared_ptr<UntilTreeFilter> Until(const TreeFilterPtr& filter) { return std::make_shared<UntilTreeFilter>(filter); }
   inline std::shared_ptr<ContainsTreeFilter> Contains(const std::wstring& text) { return std::make_shared<ContainsTreeFilter>(text); }
   inline std::shared_ptr<RegexTreeFilter> Regex(const wchar_t* reg) { return std::make_shared<RegexTreeFilter>(reg ? reg : L""); }
   inline std::shared_ptr<RegexTreeFilter> Regex(const std::wstring& reg) { return std::make_shared<RegexTreeFilter>(reg); }
   inline std::shared_ptr<NotTreeFilter> Not(const TreeFilterPtr& filter) { return std::make_shared<NotTreeFilter>(filter); }
   inline std::shared_ptr<OrTreeFilter> Or(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) { return std::make_shared<OrTreeFilter>(lhs, rhs); }
   inline std::shared_ptr<AndTreeFilter> And(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) { return std::make_shared<AndTreeFilter>(lhs, rhs); }
   inline std::shared_ptr<OrTreeFilter> Any(const std::vector<TreeFilterPtr>& filters) { return std::make_shared<OrTreeFilter>(filters); }
   inline std::shared_ptr<AndTreeFilter> All(const std::vector<TreeFilterPtr>& filters) { return std::make_shared<AndTreeFilter>(filters); }
   inline std::shared_ptr<UnderTreeFilter> Under(const TreeFilterPtr& filter, bool includeSelf = true) { return std::make_shared<UnderTreeFilter>(filter, includeSelf); }
   inline std::shared_ptr<RemoveChildrenTreeFilter> NoChild(const TreeFilterPtr& filter, bool removeSelf = false) { return std::make_shared<RemoveChildrenTreeFilter>(filter, removeSelf); }
   inline std::shared_ptr<LevelRangeTreeFilter> LevelRange(size_t min, size_t max) { return std::make_shared<LevelRangeTreeFilter>(min, max); }
   inline std::shared_ptr<LevelRangeTreeFilter> MinLevel(size_t level) { return LevelRange(level, -1); }
   inline std::shared_ptr<LevelRangeTreeFilter> MaxLevel(size_t level) { return LevelRange(0, level); }
   inline std::shared_ptr<IfSubTreeTreeFilter> IfSubTree(const TreeFilterPtr& filter) { return std::make_shared<IfSubTreeTreeFilter>(filter); }
   inline std::shared_ptr<IfSiblingTreeFilter> IfSibling(const TreeFilterPtr& filter) { return std::make_shared<IfSiblingTreeFilter>(filter); }

   // Filters a source tree into a filtered tree using the given filter.

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const TreeFilterPtr& filter);

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
      std::vector<TextTree::Node *> _filteredBranchNodes;
      std::vector<bool> _fillChildren;
   };

}

