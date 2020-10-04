#include "dak/tree_reader/tree_filter.h"
#include "dak/tree_reader/tree_filtering.h"
#include "dak/tree_reader/tree_filter_helpers.h"
#include "dak/tree_reader/text_tree_visitor.h"
#include "dak/tree_reader/named_filters.h"

#include <sstream>

namespace dak::tree_reader
{
   using namespace std;
   using result = tree_filter_t::result_t;
   using node = text_tree_t::node_t;

   constexpr result keep { false, false, true };
   constexpr result drop { false, false, false };
   constexpr result stop_and_keep { true, false, true };
   constexpr result stop_and_drop { true, false, false };
   constexpr result drop_and_skip { false, true, false };
   constexpr result keep_and_skip { false, true, true };

   wstring tree_filter_t::get_name() const
   {
      return get_short_name();
   }

   delegate_tree_filter_t::delegate_tree_filter_t(const delegate_tree_filter_t& other)
   : sub_filter(other.sub_filter)
   {
      if (sub_filter)
         sub_filter = sub_filter->clone();
   }

   result delegate_tree_filter_t::is_kept(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
   {
      if (!sub_filter)
         return keep;

      return sub_filter->is_kept(tree, node, level);
   }

   result accept_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      return keep;
   }

   result stop_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      return keep ? stop_and_keep : stop_and_drop;
   }

   result until_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      return delegate_tree_filter_t::is_kept(tree, node, level).keep ? stop_and_drop : drop;
   }

   result contains_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      return (wcsstr(node.text_ptr, contained.c_str()) != nullptr) ? keep : drop;
   }
   
   int unique_tree_filter_t::hash::operator()(const wchar_t* text) const
   {
      int h = 0;  if (text) while (*text) { h += *text * 131; ++text; } return h & INT_MAX;
   }

   bool unique_tree_filter_t::comp::operator()(const wchar_t* lhs, const wchar_t* rhs) const
   {
      return std::wcscmp(lhs, rhs) == 0;
   }

   result unique_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      if (_uniques.count(node.text_ptr) > 0)
         return drop;

      _uniques.insert(node.text_ptr);
      return keep;
   }

   result text_address_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      return (exact_address == node.text_ptr) ? keep : drop;
   }

   result regex_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      return regex_search(node.text_ptr, regex) ? keep : drop;
   }

   combine_tree_filter_t::combine_tree_filter_t(const combine_tree_filter_t& other)
   : filters(other.filters)
   {
      for (auto& filter : filters)
         if (filter)
            filter = filter->clone();
   }

   result not_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      result_t result = delegate_tree_filter_t::is_kept(tree, node, level);
      result.keep = !result.keep;
      return result;
   }

   result or_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      result_t result = drop;
      for (const auto& filter : filters)
         if (filter)
            if (result = result | filter->is_kept(tree, node, level); result.keep)
               break;
      return result;
   }

   result and_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      result_t result = keep;
      for (const auto& filter : filters)
         if (filter)
            if (result = result & filter->is_kept(tree, node, level); !result.keep)
               break;
      return result;
   }

   result under_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      // If we have found a match previously for the under filter,
      // then keep the nodes while we're still in levels deeper
      // than where we found the match.
      if (level > _keep_all_nodes_under_level)
         return keep;

      // If we've reached back up to the level where we found the match previously,
      // then stop keeping nodes. We do this by making the apply under level very large.
      if (level <= _keep_all_nodes_under_level)
         _keep_all_nodes_under_level = -1;

      // If the node doesn't match the under filter, don't apply the other filter.
      // Just return the result of the other filter.
      result_t result = delegate_tree_filter_t::is_kept(tree, node, level);
      if (!result.keep)
         return result;

      // If we reach here, the under filter matched, so we will start accepting
      // the nodes under this one.
      //
      // Record the level at which we must come back up to to stop accepting nodes
      // without checking the filter.
      _keep_all_nodes_under_level = level;

      // If the filter must not keep the matching node, then set keep to false here.
      // apply it here.
      if (!include_self)
         result.keep = false;

      return result;
   }

   result remove_children_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      if (!delegate_tree_filter_t::is_kept(tree, node, level).keep)
         return keep;

      return include_self ? drop_and_skip : keep_and_skip;
   }

   result level_range_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      if (level < min_level)
         return drop;

      if (level <= max_level)
         return keep;

      return drop_and_skip;
   }

   result stop_when_kept_tree_filter_t::is_kept(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
   {
      result_t result = delegate_tree_filter_t::is_kept(tree, node, level);
      if (result.keep)
         result = result | stop_and_keep;

      return result;
   }

   result if_subtree_tree_filter_t::is_kept(const text_tree_t& tree, const node& node, size_t level)
   {
      stop_when_kept_tree_filter_t stop_when_kept(sub_filter);
      filter_tree_visitor_t visitor(tree, _filtered, stop_when_kept);
      visit_in_order(tree, &node, false, visitor);
      return (_filtered.roots.size() > 0) ? keep : drop;
   }

   result if_sibling_tree_filter_t::is_kept(const text_tree_t& tree, const node& a_node, size_t level)
   {
      const auto& children = a_node.parent ? a_node.parent->children : tree.roots;
      auto pos = children.begin();
      pos += a_node.index_in_parent;

      for (; pos != children.end(); ++pos)
      {
         const node& c_node = **pos;
         const auto result = delegate_tree_filter_t::is_kept(tree, c_node, level);
         if (result.keep)
            return keep;
         if (result.stop)
            break;
      }

      return drop;
   }

   result named_tree_filter_t::is_kept(const text_tree_t& tree, const text_tree_t::node_t& node, size_t level)
   {
      if (!filter)
         return keep;

      return filter->is_kept(tree, node, level);
   }

   #define IMPLEMENT_SIMPLE_NAME(cl, name, desc)      \
      wstring cl::get_short_name() const              \
      {                                               \
         return name;                                 \
      }                                               \
                                                      \
      wstring cl::get_description() const             \
      {                                               \
         return desc;                                 \
      }

   #define IMPLEMENT_STREAM_NAME(cl, sn, fn, desc)    \
      wstring cl::get_short_name() const              \
      {                                               \
         return sn;                                   \
      }                                               \
                                                      \
      wstring cl::get_name() const                    \
      {                                               \
         wostringstream sstream;                      \
         sstream << sn L" " << fn;                    \
         return sstream.str();                        \
      }                                               \
                                                      \
      wstring cl::get_description() const             \
      {                                               \
         return desc;                                 \
      }


   IMPLEMENT_SIMPLE_NAME(accept_tree_filter_t,          L"Do nothing",                      L"keeps all nodes")
   IMPLEMENT_SIMPLE_NAME(stop_tree_filter_t,            L"stop",                            L"stops filtering")
   IMPLEMENT_SIMPLE_NAME(until_tree_filter_t,           L"until",                           L"stops filtering when the sub-filter accepts a node")
   IMPLEMENT_STREAM_NAME(contains_tree_filter_t,        L"Match", contained,                L"keeps the node if it matches the given text")
   IMPLEMENT_SIMPLE_NAME(unique_tree_filter_t,          L"unique",                          L"keeps unique nodes (remove duplicates)")
   IMPLEMENT_STREAM_NAME(text_address_tree_filter_t,    L"Exact node", exact_address,       L"keeps one exact, previously selected node")
   IMPLEMENT_STREAM_NAME(regex_tree_filter_t,           L"Match regex", regex_text,         L"keeps the node if it matches the given regular expression")
   IMPLEMENT_SIMPLE_NAME(not_tree_filter_t,             L"not",                             L"Inverses the result of the sub-filter")
   IMPLEMENT_SIMPLE_NAME(or_tree_filter_t,              L"If any",                          L"keeps the node if any of the sub-filters accepts")
   IMPLEMENT_SIMPLE_NAME(and_tree_filter_t,             L"If all",                          L"keeps the node if all of the sub-filters accepts")
   IMPLEMENT_SIMPLE_NAME(under_tree_filter_t,           L"If, keep all under",              L"keeps all nodes under")
   IMPLEMENT_SIMPLE_NAME(remove_children_tree_filter_t, L"remove all children",             L"removes all children nodes")
   IMPLEMENT_STREAM_NAME(level_range_tree_filter_t,     L"keep levels", min_level << L"-" << max_level, L"keeps nodes that are within a range of tree depths")
   IMPLEMENT_SIMPLE_NAME(if_subtree_tree_filter_t,      L"If a child",                      L"keeps the node if one if its child is accepted by the sub-filter")
   IMPLEMENT_SIMPLE_NAME(if_sibling_tree_filter_t,      L"If a sibling",                    L"keeps the node if one if its sibling is accepted by the sub-filter")
   IMPLEMENT_SIMPLE_NAME(named_tree_filter_t,           name,                               L"Delegates the decision to keep the node to the named filter")
   IMPLEMENT_SIMPLE_NAME(stop_when_kept_tree_filter_t,  L"stop when kept",                  L"stops filtering when a sub-filter keeps a node.")

   #undef IMPLEMENT_SIMPLE_NAME
   #undef IMPLEMENT_STREAM_NAME

   #define IMPLEMENT_CLONE(cl)                        \
      tree_filter_ptr_t cl::clone() const               \
      {                                               \
         return make_shared<cl>(*this);               \
      }                                               \

   IMPLEMENT_CLONE(accept_tree_filter_t)
   IMPLEMENT_CLONE(stop_tree_filter_t)
   IMPLEMENT_CLONE(until_tree_filter_t)
   IMPLEMENT_CLONE(contains_tree_filter_t)
   IMPLEMENT_CLONE(unique_tree_filter_t)
   IMPLEMENT_CLONE(text_address_tree_filter_t)
   IMPLEMENT_CLONE(regex_tree_filter_t)
   IMPLEMENT_CLONE(not_tree_filter_t)
   IMPLEMENT_CLONE(or_tree_filter_t)
   IMPLEMENT_CLONE(and_tree_filter_t)
   IMPLEMENT_CLONE(under_tree_filter_t)
   IMPLEMENT_CLONE(remove_children_tree_filter_t)
   IMPLEMENT_CLONE(level_range_tree_filter_t)
   IMPLEMENT_CLONE(if_subtree_tree_filter_t)
   IMPLEMENT_CLONE(if_sibling_tree_filter_t)
   IMPLEMENT_CLONE(named_tree_filter_t)
   IMPLEMENT_CLONE(stop_when_kept_tree_filter_t)

   #undef IMPLEMENT_CLONE
}
