#include "TreeFilterHelpers.h"

#include <typeindex>

namespace TreeReader
{
   using namespace std;

   namespace
   {
      namespace L
      {
         inline const wchar_t* t(const wchar_t* text)
         {
            return text;
         }
      }

      static struct
      {
         std::type_index type;
         const wchar_t* name;
         const wchar_t* desc;
      }
      FilterNames[] =
      {
         { typeid(AcceptTreeFilter),         L"do nothing",      L"keeps all nodes" },
         { typeid(StopTreeFilter),           L"stop",            L"stops filtering" },
         { typeid(UntilTreeFilter),          L"until",           L"stops filtering when the sub-filter accepts a node" },
         { typeid(ContainsTreeFilter),       L"match text",      L"keeps the node if it matches the given text" },
         { typeid(TextAddressTreeFilter),    L"exact node",      L"keeps one exact, previously selected node" },
         { typeid(RegexTreeFilter),          L"match regex",     L"keeps the node if it matches the given regular expression" },
         { typeid(NotTreeFilter),            L"not",             L"inverses the result of the sub-filter" },
         { typeid(OrTreeFilter),             L"any",             L"keeps the node if any of the sub-filters accepts" },
         { typeid(AndTreeFilter),            L"all",             L"keeps the node if all of the sub-filters accepts" },
         { typeid(UnderTreeFilter),          L"keep node under", L"keeps all nodes under" },
         { typeid(CountSiblingsTreeFilter),  L"limit siblings",  L"keeps a maximum number of sibling nodes" },
         { typeid(CountChildrenTreeFilter),  L"limit children",  L"keeps a maximum number of children nodes" },
         { typeid(RemoveChildrenTreeFilter), L"remove children", L"removes all children nodes" },
         { typeid(LevelRangeTreeFilter),     L"level range",     L"keeps nodes that are within a range of tree depths" },
         { typeid(IfSubTreeTreeFilter),      L"if child",        L"keeps the node if one if its child is accepted by the sub-filter" },
         { typeid(IfSiblingTreeFilter),      L"if sibling",      L"keeps the node if one if its sibling is accepted by the sub-filter" },
         { typeid(NamedTreeFilter),          L"named filter",    L"defers the decision to keep the node to the named filter" },
      };
   }

   const wchar_t* GetFilterName(const TreeFilter& filter)
   {
      for (const auto& item : FilterNames)
         if (std::type_index(typeid(filter)) == item.type)
            return L::t(item.name);
      return L::t(L"unknown");
   }

   const wchar_t* GetFilterDescription(const TreeFilter& filter)
   {
      for (const auto& item : FilterNames)
         if (std::type_index(typeid(filter)) == item.type)
            return L::t(item.desc);
      return L::t(L"unknown node type");
   }

   bool VisitFilters(TreeFilter* filter, FilterVisitFunction func)
   {
      if (!func(filter))
         return false;

      if (auto delegate = dynamic_cast<DelegateTreeFilter *>(filter))
      {
         if (!VisitFilters(delegate->Filter.get(), func))
            return false;
      }
      else if (auto combined = dynamic_cast<CombineTreeFilter *>(filter))
      {
         for (auto& child : combined->Filters)
         {
            if (!VisitFilters(child.get(), func))
               return false;
         }
      }

      return true;
   }
}

