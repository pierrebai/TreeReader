#include "TreeFilter.h"
#include "TreeFiltering.h"
#include "TreeFilterHelpers.h"
#include "TextTreeVisitor.h"

#include <sstream>

namespace TreeReader
{
   using namespace std;
   using Result = TreeFilter::Result;
   using Node = TextTree::Node;

   constexpr Result Keep { false, false, true };
   constexpr Result Drop { false, false, false };
   constexpr Result StopAndKeep { true, false, true };
   constexpr Result StopAndDrop { true, false, false };
   constexpr Result DropAndSkip { false, true, false };
   constexpr Result KeepAndSkip { false, true, true };

   wstring TreeFilter::GetName() const
   {
      return GetShortName();
   }

   DelegateTreeFilter::DelegateTreeFilter(const DelegateTreeFilter& other)
   : Filter(other.Filter)
   {
      if (Filter)
         Filter = Filter->Clone();
   }

   Result DelegateTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      return Filter->IsKept(tree, node, level);
   }

   Result AcceptTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return Keep;
   }

   Result StopTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return Keep ? StopAndKeep : StopAndDrop;
   }

   Result UntilTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return DelegateTreeFilter::IsKept(tree, node, level).Keep ? StopAndDrop : Drop;
   }

   Result ContainsTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return (wcsstr(node.TextPtr, Contained.c_str()) != nullptr) ? Keep : Drop;
   }
   
   int UniqueTreeFilter::hash::operator()(const wchar_t* text) const
   {
      int h = 0;  if (text) while (*text) { h += *text * 131; ++text; } return h & INT_MAX;
   }

   bool UniqueTreeFilter::comp::operator()(const wchar_t* lhs, const wchar_t* rhs) const
   {
      return std::wcscmp(lhs, rhs) == 0;
   }

   Result UniqueTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (_uniques.count(node.TextPtr) > 0)
         return Drop;

      _uniques.insert(node.TextPtr);
      return Keep;
   }

   Result TextAddressTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return (ExactAddress == node.TextPtr) ? Keep : Drop;
   }

   Result RegexTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      return regex_search(node.TextPtr, Regex) ? Keep : Drop;
   }

   CombineTreeFilter::CombineTreeFilter(const CombineTreeFilter& other)
   : Filters(other.Filters)
   {
      for (auto& filter : Filters)
         if (filter)
            filter = filter->Clone();
   }

   Result NotTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      Result result = DelegateTreeFilter::IsKept(tree, node, level);
      result.Keep = !result.Keep;
      return result;
   }

   Result OrTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      Result result = Drop;
      for (const auto& filter : Filters)
         if (filter)
            if (result = result | filter->IsKept(tree, node, level); result.Keep)
               break;
      return result;
   }

   Result AndTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      Result result = Keep;
      for (const auto& filter : Filters)
         if (filter)
            if (result = result & filter->IsKept(tree, node, level); !result.Keep)
               break;
      return result;
   }

   Result UnderTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      // If we have found a match previously for the under filter,
      // then keep the nodes while we're still in levels deeper
      // than where we found the match.
      if (level > _keepAllNodesUnderLevel)
         return Keep;

      // If we've reached back up to the level where we found the match previously,
      // then stop keeping nodes. We do this by making the apply under level very large.
      if (level <= _keepAllNodesUnderLevel)
         _keepAllNodesUnderLevel = -1;

      // If the node doesn't match the under filter, don't apply the other filter.
      // Just return the result of the other filter.
      Result result = DelegateTreeFilter::IsKept(tree, node, level);
      if (!result.Keep)
         return result;

      // If we reach here, the under filter matched, so we will start accepting
      // the nodes under this one.
      //
      // Record the level at which we must come back up to to stop accepting nodes
      // without checking the filter.
      _keepAllNodesUnderLevel = level;

      // If the filter must not keep the matching node, then set Keep to false here.
      // apply it here.
      if (!IncludeSelf)
         result.Keep = false;

      return result;
   }

   Result RemoveChildrenTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (!DelegateTreeFilter::IsKept(tree, node, level).Keep)
         return Keep;

      return IncludeSelf ? DropAndSkip : KeepAndSkip;
   }

   Result LevelRangeTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      if (level < MinLevel)
         return Drop;

      if (level <= MaxLevel)
         return Keep;

      return DropAndSkip;
   }

   Result StopWhenKeptTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t level)
   {
      Result result = DelegateTreeFilter::IsKept(tree, node, level);
      if (result.Keep)
         result = result | StopAndKeep;

      return result;
   }

   Result IfSubTreeTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      StopWhenKeptTreeFilter stopWhenKept(Filter);
      FilterTreeVisitor visitor(tree, _filtered, stopWhenKept);
      VisitInOrder(tree, &node, false, visitor);
      return (_filtered.Roots.size() > 0) ? Keep : Drop;
   }

   Result IfSiblingTreeFilter::IsKept(const TextTree& tree, const Node& node, size_t level)
   {
      const auto& children = node.Parent ? node.Parent->Children : tree.Roots;
      auto pos = children.begin();
      pos += node.IndexInParent;

      for (; pos != children.end(); ++pos)
      {
         const Node& node = **pos;
         const auto result = DelegateTreeFilter::IsKept(tree, node, level);
         if (result.Keep)
            return Keep;
         if (result.Stop)
            break;
      }

      return Drop;
   }

   Result NamedTreeFilter::IsKept(const TextTree& tree, const TextTree::Node& node, size_t level)
   {
      if (!Filter)
         return Keep;

      return Filter->IsKept(tree, node, level);
   }

   #define IMPLEMENT_SIMPLE_NAME(cl, name, desc)      \
      wstring cl::GetShortName() const                \
      {                                               \
         return name;                                 \
      }                                               \
                                                      \
      wstring cl::GetDescription() const              \
      {                                               \
         return desc;                                 \
      }

   #define IMPLEMENT_STREAM_NAME(cl, sn, fn, desc)    \
      wstring cl::GetShortName() const                \
      {                                               \
         return sn;                                   \
      }                                               \
                                                      \
      wstring cl::GetName() const                     \
      {                                               \
         wostringstream sstream;                      \
         sstream << sn L" " << fn;                    \
         return sstream.str();                        \
      }                                               \
                                                      \
      wstring cl::GetDescription() const              \
      {                                               \
         return desc;                                 \
      }


   IMPLEMENT_SIMPLE_NAME(AcceptTreeFilter,         L"Do nothing",                      L"Keeps all nodes")
   IMPLEMENT_SIMPLE_NAME(StopTreeFilter,           L"Stop",                            L"Stops filtering")
   IMPLEMENT_SIMPLE_NAME(UntilTreeFilter,          L"Until",                           L"Stops filtering when the sub-filter accepts a node")
   IMPLEMENT_STREAM_NAME(ContainsTreeFilter,       L"Match", Contained,                L"Keeps the node if it matches the given text")
   IMPLEMENT_SIMPLE_NAME(UniqueTreeFilter,         L"Unique",                          L"Keeps unique nodes (remove duplicates)")
   IMPLEMENT_STREAM_NAME(TextAddressTreeFilter,    L"Exact node", ExactAddress,        L"Keeps one exact, previously selected node")
   IMPLEMENT_STREAM_NAME(RegexTreeFilter,          L"Match regex", RegexTextForm,      L"Keeps the node if it matches the given regular expression")
   IMPLEMENT_SIMPLE_NAME(NotTreeFilter,            L"Not",                             L"Inverses the result of the sub-filter")
   IMPLEMENT_SIMPLE_NAME(OrTreeFilter,             L"If any",                          L"Keeps the node if any of the sub-filters accepts")
   IMPLEMENT_SIMPLE_NAME(AndTreeFilter,            L"If all",                          L"Keeps the node if all of the sub-filters accepts")
   IMPLEMENT_SIMPLE_NAME(UnderTreeFilter,          L"If, keep all under",              L"Keeps all nodes under")
   IMPLEMENT_SIMPLE_NAME(RemoveChildrenTreeFilter, L"Remove all children",             L"Removes all children nodes")
   IMPLEMENT_STREAM_NAME(LevelRangeTreeFilter,     L"Keep levels", MinLevel << L"-" << MaxLevel, L"Keeps nodes that are within a range of tree depths")
   IMPLEMENT_SIMPLE_NAME(IfSubTreeTreeFilter,      L"If a child",                      L"Keeps the node if one if its child is accepted by the sub-filter")
   IMPLEMENT_SIMPLE_NAME(IfSiblingTreeFilter,      L"If a sibling",                    L"Keeps the node if one if its sibling is accepted by the sub-filter")
   IMPLEMENT_SIMPLE_NAME(NamedTreeFilter,          Name,                               L"Delegates the decision to keep the node to the named filter")
   IMPLEMENT_SIMPLE_NAME(StopWhenKeptTreeFilter,   L"Stop when kept",                  L"Stops filtering when a sub-filter keeps a node.")

   #undef IMPLEMENT_SIMPLE_NAME
   #undef IMPLEMENT_STREAM_NAME

   #define IMPLEMENT_CLONE(cl)                        \
      TreeFilterPtr cl::Clone() const                 \
      {                                               \
         return make_shared<cl>(*this);               \
      }                                               \

   IMPLEMENT_CLONE(AcceptTreeFilter)
   IMPLEMENT_CLONE(StopTreeFilter)
   IMPLEMENT_CLONE(UntilTreeFilter)
   IMPLEMENT_CLONE(ContainsTreeFilter)
   IMPLEMENT_CLONE(UniqueTreeFilter)
   IMPLEMENT_CLONE(TextAddressTreeFilter)
   IMPLEMENT_CLONE(RegexTreeFilter)
   IMPLEMENT_CLONE(NotTreeFilter)
   IMPLEMENT_CLONE(OrTreeFilter)
   IMPLEMENT_CLONE(AndTreeFilter)
   IMPLEMENT_CLONE(UnderTreeFilter)
   IMPLEMENT_CLONE(RemoveChildrenTreeFilter)
   IMPLEMENT_CLONE(LevelRangeTreeFilter)
   IMPLEMENT_CLONE(IfSubTreeTreeFilter)
   IMPLEMENT_CLONE(IfSiblingTreeFilter)
   IMPLEMENT_CLONE(NamedTreeFilter)
   IMPLEMENT_CLONE(StopWhenKeptTreeFilter)

   #undef IMPLEMENT_CLONE
}
