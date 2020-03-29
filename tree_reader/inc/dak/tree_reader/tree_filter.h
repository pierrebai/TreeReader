#pragma once

#include "dak/tree_reader/text_tree.h"
#include "dak/tree_reader/text_tree_visitor.h"

#include <string>
#include <memory>
#include <vector>
#include <regex>
#include <future>
#include <unordered_set>

namespace dak::tree_reader
{
   struct tree_filter;
   typedef std::shared_ptr<tree_filter> tree_filter_ptr;

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter used to reduce a text tree to another simpler text tree.

   struct tree_filter
   {
      // Possible result of a filter on a node: stop or not, visit children or not, keep the node or not.

      struct result : tree_visitor::result
      {
         bool keep = false;

         // Combining results preserves the stop and skip children flags, but combines the keep flags as specified: or, and.

         result operator|(const result& r) const
         {
            return result{ stop || r.stop, skip_children || r.skip_children, keep || r.keep };
         }

         result operator&(const result& r) const
         {
            return result{ stop || r.stop, skip_children || r.skip_children, keep && r.keep };
         }
      };

      virtual ~tree_filter() {};

      // filter a node to decide to keep drop the node.
      virtual result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) = 0;

      // gets the name of the node, including its data.
      virtual std::wstring get_name() const;

      // gets the short name of the node, without its data.
      virtual std::wstring get_short_name() const = 0;

      // gets a longer description of the purpose of the node.
      virtual std::wstring get_description() const = 0;

      // Create a copy of this filter.
      virtual tree_filter_ptr clone() const = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that delegates to another filter.
   //
   // allows adding behavior to another existing filter.

   struct delegate_tree_filter : tree_filter
   {
      tree_filter_ptr filter;

      delegate_tree_filter() = default;
      delegate_tree_filter(const tree_filter_ptr& filter) : filter(filter) { }
      delegate_tree_filter(const delegate_tree_filter& other);

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that accepts all nodes.

   struct accept_tree_filter : tree_filter
   {
      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that stops filtering.
   //
   // Can keep or not keep the node, as desired.

   struct stop_tree_filter : tree_filter
   {
      bool keep = true;

      stop_tree_filter() = default;
      stop_tree_filter(bool keep) : keep(keep) {}

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that stops the filtering as soon as its delegate accept a node.

   struct stop_when_kept_tree_filter : delegate_tree_filter
   {
      stop_when_kept_tree_filter() = default;
      stop_when_kept_tree_filter(const tree_filter_ptr& filter) : delegate_tree_filter(filter) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that stops filtering when another sub-filter keeps a node.
   //
   // Never keeps the node. Used to stop doing sb-tree filtering. (See if_subtree and if_sibling.)

   struct until_tree_filter : delegate_tree_filter
   {
      until_tree_filter() = default;
      until_tree_filter(const tree_filter_ptr& filter) : delegate_tree_filter(filter) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that keeps nodes containing a given text.

   struct contains_tree_filter : tree_filter
   {
      std::wstring Contained;

      contains_tree_filter() = default;
      contains_tree_filter(const std::wstring& text) : Contained(text) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_name() const override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that keeps only the first node that contain some text.

   struct unique_tree_filter : tree_filter
   {
      unique_tree_filter() = default;

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;

   private:
      struct hash { int  operator()(const wchar_t* text) const; };
      struct comp { bool operator()(const wchar_t* lhs, const wchar_t* rhs) const; };

      std::unordered_set<const wchar_t*, hash, comp> _uniques;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter by matching the exact address of the text.
   // Can be use to keep an exact node, using selection in a UI for example.

   struct text_address_tree_filter : tree_filter
   {
      const wchar_t* exact_address = nullptr;

      text_address_tree_filter() = default;
      text_address_tree_filter(const wchar_t* addr) : exact_address(addr) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_name() const override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that keeps nodes matching a regular expression.

   struct regex_tree_filter : tree_filter
   {
      std::wstring regexTextForm;
      std::wregex regex;

      regex_tree_filter() = default;
      regex_tree_filter(const std::wstring& reg) : regexTextForm(reg), regex(std::wregex(reg)) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_name() const override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that combines the result of other filters.

   struct combine_tree_filter : tree_filter
   {
      std::vector<tree_filter_ptr> named_filters;

      combine_tree_filter() = default;

      combine_tree_filter(const tree_filter_ptr& lhs, const tree_filter_ptr& rhs) { named_filters.push_back(lhs); named_filters.push_back(rhs); }
      combine_tree_filter(const std::vector<tree_filter_ptr>& filters) : named_filters(filters) {}
      combine_tree_filter(const combine_tree_filter& other);
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that inverts the keep decision of another filter.

   struct not_tree_filter : delegate_tree_filter
   {
      not_tree_filter() = default;
      not_tree_filter(const tree_filter_ptr& filter) : delegate_tree_filter(filter) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that accepts a node if any of its sub-filters accept the node.

   struct or_tree_filter : combine_tree_filter
   {
      or_tree_filter() = default;
      or_tree_filter(const tree_filter_ptr& lhs, const tree_filter_ptr& rhs) : combine_tree_filter(lhs, rhs) { }
      or_tree_filter(const std::vector<tree_filter_ptr>& filters) : combine_tree_filter(filters) {}

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that accepts a node if all of its sub-filters accept the node.

   struct and_tree_filter : combine_tree_filter
   {
      and_tree_filter() = default;
      and_tree_filter(const tree_filter_ptr& lhs, const tree_filter_ptr& rhs) : combine_tree_filter(lhs, rhs) { }
      and_tree_filter(const std::vector<tree_filter_ptr>& filters) : combine_tree_filter(filters) {}

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that accepts all children of a node that was accepted by another.
   //
   // Can accept or not that parent initial node.

   struct under_tree_filter : delegate_tree_filter
   {
      bool include_self = true;

      under_tree_filter() = default;
      under_tree_filter(const tree_filter_ptr& filter, bool includeSelf = true)
         : delegate_tree_filter(filter), include_self(includeSelf) {}

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;

   private:
      size_t _keep_all_nodes_under_level = -1;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that removes all children of a node that was accepted
   // by another filter.
   //
   // Can remove or not that parent initial node.

   struct remove_children_tree_filter : delegate_tree_filter
   {
      bool include_self = false;

      remove_children_tree_filter() = default;
      remove_children_tree_filter(const tree_filter_ptr& filter, bool removeSelf)
         : delegate_tree_filter(filter), include_self(removeSelf) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that accepts nodes that are within a range of depth in the tree.

   struct level_range_tree_filter : tree_filter
   {
      size_t min_level;
      size_t max_level;

      level_range_tree_filter() = default;
      level_range_tree_filter(size_t minLevel, size_t maxLevel) : min_level(minLevel), max_level(maxLevel) {}

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_name() const override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that accepts a node if at least one child is accepted
   // by another filter.

   struct if_subtree_tree_filter : delegate_tree_filter
   {
      if_subtree_tree_filter() = default;
      if_subtree_tree_filter(const tree_filter_ptr& filter) : delegate_tree_filter(filter) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;

   private:
      text_tree _filtered;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // filter that accepts a node if at least one sibling is accepted
   // by another filter.

   struct if_sibling_tree_filter : delegate_tree_filter
   {
      if_sibling_tree_filter() = default;
      if_sibling_tree_filter(const tree_filter_ptr& filter) : delegate_tree_filter(filter) { }

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // functions to create filters.

   inline std::shared_ptr<accept_tree_filter> accept() { return std::make_shared<accept_tree_filter>(); }
   inline std::shared_ptr<stop_tree_filter> stop() { return std::make_shared<stop_tree_filter>(); }
   inline std::shared_ptr<until_tree_filter> until(const tree_filter_ptr& filter) { return std::make_shared<until_tree_filter>(filter); }
   inline std::shared_ptr<contains_tree_filter> contains(const std::wstring& text) { return std::make_shared<contains_tree_filter>(text); }
   inline std::shared_ptr<unique_tree_filter> unique() { return std::make_shared<unique_tree_filter>(); }
   inline std::shared_ptr<text_address_tree_filter> exact_address(const wchar_t* text) { return std::make_shared<text_address_tree_filter>(text); }
   inline std::shared_ptr<regex_tree_filter> regex(const wchar_t* reg) { return std::make_shared<regex_tree_filter>(reg ? reg : L""); }
   inline std::shared_ptr<regex_tree_filter> regex(const std::wstring& reg) { return std::make_shared<regex_tree_filter>(reg); }
   inline std::shared_ptr<not_tree_filter> not(const tree_filter_ptr& filter) { return std::make_shared<not_tree_filter>(filter); }
   inline std::shared_ptr<or_tree_filter> or(const tree_filter_ptr& lhs, const tree_filter_ptr& rhs) { return std::make_shared<or_tree_filter>(lhs, rhs); }
   inline std::shared_ptr<and_tree_filter> and(const tree_filter_ptr& lhs, const tree_filter_ptr& rhs) { return std::make_shared<and_tree_filter>(lhs, rhs); }
   inline std::shared_ptr<or_tree_filter> Any(const std::vector<tree_filter_ptr>& filters) { return std::make_shared<or_tree_filter>(filters); }
   inline std::shared_ptr<and_tree_filter> all(const std::vector<tree_filter_ptr>& filters) { return std::make_shared<and_tree_filter>(filters); }
   inline std::shared_ptr<under_tree_filter> under(const tree_filter_ptr& filter, bool includeSelf = true) { return std::make_shared<under_tree_filter>(filter, includeSelf); }
   inline std::shared_ptr<remove_children_tree_filter> no_child(const tree_filter_ptr& filter, bool removeSelf = false) { return std::make_shared<remove_children_tree_filter>(filter, removeSelf); }
   inline std::shared_ptr<level_range_tree_filter> level_range(size_t min, size_t max) { return std::make_shared<level_range_tree_filter>(min, max); }
   inline std::shared_ptr<level_range_tree_filter> min_level(size_t level) { return level_range(level, -1); }
   inline std::shared_ptr<level_range_tree_filter> max_level(size_t level) { return level_range(0, level); }
   inline std::shared_ptr<if_subtree_tree_filter> if_subtree(const tree_filter_ptr& filter) { return std::make_shared<if_subtree_tree_filter>(filter); }
   inline std::shared_ptr<if_sibling_tree_filter> if_sibling(const tree_filter_ptr& filter) { return std::make_shared<if_sibling_tree_filter>(filter); }
   inline std::shared_ptr<stop_when_kept_tree_filter> stop_when_kept(const tree_filter_ptr& filter) { return std::make_shared<stop_when_kept_tree_filter>(filter); }
}

