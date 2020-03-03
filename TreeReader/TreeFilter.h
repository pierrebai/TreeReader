#pragma once

#include "TextTree.h"
#include "TextTreeVisitor.h"

#include <string>
#include <memory>
#include <vector>
#include <regex>
#include <future>

namespace TreeReader
{
   struct TreeFilter;
   typedef std::shared_ptr<TreeFilter> TreeFilterPtr;

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

      // Filter a node to decide to keep drop the node.
      virtual Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) = 0;

      // Gets the name of the node, including its data.
      virtual std::wstring GetName() const;

      // Gets the short name of the node, without its data.
      virtual std::wstring GetShortName() const = 0;

      // Gets a longer description of the purpose of the node.
      virtual std::wstring GetDescription() const = 0;

      // Create a copy of this filter.
      virtual TreeFilterPtr Clone() const = 0;
   };

   // Filter that delegates to another filter.
   //
   // Allows adding behavior to another existing filter.

   struct DelegateTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;

      DelegateTreeFilter() = default;
      DelegateTreeFilter(const TreeFilterPtr& filter) : Filter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
   };

   // Filter that accepts all nodes.

   struct AcceptTreeFilter : TreeFilter
   {
      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
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
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that stops filtering when another sub-filter keeps a node.
   //
   // Never keeps the node. Used to stop doing sb-tree filtering. (See IfSubTree and IfSibling.)

   struct UntilTreeFilter : DelegateTreeFilter
   {
      UntilTreeFilter() = default;
      UntilTreeFilter(const TreeFilterPtr& filter) : DelegateTreeFilter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that keeps nodes containing a given text.

   struct ContainsTreeFilter : TreeFilter
   {
      std::wstring Contained;

      ContainsTreeFilter() = default;
      ContainsTreeFilter(const std::wstring& text) : Contained(text) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetName() const override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter by matching the exact address of the text.
   // Can be use to keep an exact node, using selection in a UI for example.

   struct TextAddressTreeFilter : TreeFilter
   {
      const wchar_t* ExactAddress = nullptr;

      TextAddressTreeFilter() = default;
      TextAddressTreeFilter(const wchar_t* addr) : ExactAddress(addr) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetName() const override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that keeps nodes matching a regular expression.

   struct RegexTreeFilter : TreeFilter
   {
      std::wstring RegexTextForm;
      std::wregex Regex;

      RegexTreeFilter() = default;
      RegexTreeFilter(const std::wstring& reg) : RegexTextForm(reg), Regex(std::wregex(reg)) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetName() const override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
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

   struct NotTreeFilter : DelegateTreeFilter
   {
      NotTreeFilter() = default;
      NotTreeFilter(const TreeFilterPtr& filter) : DelegateTreeFilter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that accepts a node if any of its sub-filters accept the node.

   struct OrTreeFilter : CombineTreeFilter
   {
      OrTreeFilter() = default;
      OrTreeFilter(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) : CombineTreeFilter(lhs, rhs) { }
      OrTreeFilter(const std::vector<TreeFilterPtr>& filters) : CombineTreeFilter(filters) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that accepts a node if all of its sub-filters accept the node.

   struct AndTreeFilter : CombineTreeFilter
   {
      AndTreeFilter() = default;
      AndTreeFilter(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) : CombineTreeFilter(lhs, rhs) { }
      AndTreeFilter(const std::vector<TreeFilterPtr>& filters) : CombineTreeFilter(filters) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that accepts all children of a node that was accepted by another filter.
   //
   // Can accept or not that parent initial node.

   struct UnderTreeFilter : DelegateTreeFilter
   {
      bool IncludeSelf = true;

      UnderTreeFilter() = default;
      UnderTreeFilter(const TreeFilterPtr& filter, bool includeSelf = true)
         : DelegateTreeFilter(filter), IncludeSelf(includeSelf) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;

   private:
      size_t _keepAllNodesUnderLevel = -1;
   };

   // Filter that accepts a given number of siblings of a node that was accepted by another filter.
   //
   // Can accept or not that initial node. That node is not included in the count in all cases.

   struct CountSiblingsTreeFilter : DelegateTreeFilter
   {
      size_t Count = 0;
      bool IncludeSelf = true;

      CountSiblingsTreeFilter() = default;
      CountSiblingsTreeFilter(const TreeFilterPtr& filter, size_t count, bool includeSelf = true)
         : DelegateTreeFilter(filter), Count(count), IncludeSelf(includeSelf) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetName() const override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;

   private:
      size_t _keepNodesAtLevel = -1;
      size_t _countdown = 0;
   };

   // Filter that accepts a given number of children of a node that was accepted by another filter.
   //
   // Can accept or not that initial node. That node is not included in the count in all cases.

   struct CountChildrenTreeFilter : DelegateTreeFilter
   {
      size_t Count = 0;
      bool IncludeSelf = true;

      CountChildrenTreeFilter() = default;
      CountChildrenTreeFilter(const TreeFilterPtr& filter, size_t count, bool includeSelf = true)
         : DelegateTreeFilter(filter), Count(count), IncludeSelf(includeSelf) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetName() const override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;

   private:
      size_t _keepNodesUnderLevel = -1;
      size_t _countdown = 0;
   };

   // Filter that removes all children of a node that was accepted by another filter.
   //
   // Can remove or not that parent initial node.

   struct RemoveChildrenTreeFilter : DelegateTreeFilter
   {
      bool IncludeSelf = false;

      RemoveChildrenTreeFilter() = default;
      RemoveChildrenTreeFilter(const TreeFilterPtr& filter, bool removeSelf)
         : DelegateTreeFilter(filter), IncludeSelf(removeSelf) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that accepts nodes that are within a range of depth in the tree.

   struct LevelRangeTreeFilter : TreeFilter
   {
      size_t MinLevel;
      size_t MaxLevel;

      LevelRangeTreeFilter() = default;
      LevelRangeTreeFilter(size_t minLevel, size_t maxLevel) : MinLevel(minLevel), MaxLevel(maxLevel) {}

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetName() const override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that accepts a node if at least one child is accepted by another filter.

   struct IfSubTreeTreeFilter : DelegateTreeFilter
   {
      IfSubTreeTreeFilter() = default;
      IfSubTreeTreeFilter(const TreeFilterPtr& filter) : DelegateTreeFilter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;

   private:
      TextTree _filtered;
   };

   // Filter that accepts a node if at least one sibling is accepted by another filter.

   struct IfSiblingTreeFilter : DelegateTreeFilter
   {
      IfSiblingTreeFilter() = default;
      IfSiblingTreeFilter(const TreeFilterPtr& filter) : DelegateTreeFilter(filter) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Filter that reference a named filter.
   //
   // Note: this is not a delegate filter because we don't want its sub-filters visible to other code.

   struct NamedTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;
      std::wstring Name;

      NamedTreeFilter() = default;
      NamedTreeFilter(const std::wstring& name) : Name(name) { }
      NamedTreeFilter(const TreeFilterPtr& filter, const std::wstring& name) : Filter(filter), Name(name) { }

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;
   };

   // Functions to create filters.

   inline std::shared_ptr<AcceptTreeFilter> Accept() { return std::make_shared<AcceptTreeFilter>(); }
   inline std::shared_ptr<StopTreeFilter> Stop() { return std::make_shared<StopTreeFilter>(); }
   inline std::shared_ptr<UntilTreeFilter> Until(const TreeFilterPtr& filter) { return std::make_shared<UntilTreeFilter>(filter); }
   inline std::shared_ptr<ContainsTreeFilter> Contains(const std::wstring& text) { return std::make_shared<ContainsTreeFilter>(text); }
   inline std::shared_ptr<TextAddressTreeFilter> ExactAddress(const wchar_t* text) { return std::make_shared<TextAddressTreeFilter>(text); }
   inline std::shared_ptr<RegexTreeFilter> Regex(const wchar_t* reg) { return std::make_shared<RegexTreeFilter>(reg ? reg : L""); }
   inline std::shared_ptr<RegexTreeFilter> Regex(const std::wstring& reg) { return std::make_shared<RegexTreeFilter>(reg); }
   inline std::shared_ptr<NotTreeFilter> Not(const TreeFilterPtr& filter) { return std::make_shared<NotTreeFilter>(filter); }
   inline std::shared_ptr<OrTreeFilter> Or(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) { return std::make_shared<OrTreeFilter>(lhs, rhs); }
   inline std::shared_ptr<AndTreeFilter> And(const TreeFilterPtr& lhs, const TreeFilterPtr& rhs) { return std::make_shared<AndTreeFilter>(lhs, rhs); }
   inline std::shared_ptr<OrTreeFilter> Any(const std::vector<TreeFilterPtr>& filters) { return std::make_shared<OrTreeFilter>(filters); }
   inline std::shared_ptr<AndTreeFilter> All(const std::vector<TreeFilterPtr>& filters) { return std::make_shared<AndTreeFilter>(filters); }
   inline std::shared_ptr<UnderTreeFilter> Under(const TreeFilterPtr& filter, bool includeSelf = true) { return std::make_shared<UnderTreeFilter>(filter, includeSelf); }
   inline std::shared_ptr<CountSiblingsTreeFilter> CountSiblings(const TreeFilterPtr& filter, size_t count, bool includeSelf = true) { return std::make_shared<CountSiblingsTreeFilter>(filter, count, includeSelf); }
   inline std::shared_ptr<CountChildrenTreeFilter> CountChildren(const TreeFilterPtr& filter, size_t count, bool includeSelf = true) { return std::make_shared<CountChildrenTreeFilter>(filter, count, includeSelf); }
   inline std::shared_ptr<RemoveChildrenTreeFilter> NoChild(const TreeFilterPtr& filter, bool removeSelf = false) { return std::make_shared<RemoveChildrenTreeFilter>(filter, removeSelf); }
   inline std::shared_ptr<LevelRangeTreeFilter> LevelRange(size_t min, size_t max) { return std::make_shared<LevelRangeTreeFilter>(min, max); }
   inline std::shared_ptr<LevelRangeTreeFilter> MinLevel(size_t level) { return LevelRange(level, -1); }
   inline std::shared_ptr<LevelRangeTreeFilter> MaxLevel(size_t level) { return LevelRange(0, level); }
   inline std::shared_ptr<IfSubTreeTreeFilter> IfSubTree(const TreeFilterPtr& filter) { return std::make_shared<IfSubTreeTreeFilter>(filter); }
   inline std::shared_ptr<IfSiblingTreeFilter> IfSibling(const TreeFilterPtr& filter) { return std::make_shared<IfSiblingTreeFilter>(filter); }
   inline std::shared_ptr<NamedTreeFilter> Named(const std::wstring& name, const TreeFilterPtr& filter = {}) { return std::make_shared<NamedTreeFilter>(filter, name); }

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

   // Filters a source tree into a filtered tree using the given filter.

   void FilterTree(const TextTree& sourceTree, TextTree& filteredTree, const TreeFilterPtr& filter);

   std::pair<std::future<TextTree>, std::shared_ptr<CanAbortTreeVisitor>> FilterTreeAsync(
      const std::shared_ptr<TextTree>& sourceTree, const TreeFilterPtr& filter);
}

