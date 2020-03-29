#include "TreeFilterHelpers.h"

namespace TreeReader
{
   using namespace std;

   bool VisitFilters(const TreeFilterPtr& filter, bool includeSelf, FilterVisitFunction func)
   {
      if (!filter)
         return true;

      if (includeSelf && !func(filter))
         return false;

      if (auto delegate = dynamic_pointer_cast<DelegateTreeFilter>(filter))
      {
         if (!VisitFilters(delegate->Filter, func))
            return false;
      }
      else if (auto combined = dynamic_pointer_cast<CombineTreeFilter>(filter))
      {
         for (auto& child : combined->Filters)
         {
            if (!VisitFilters(child, func))
               return false;
         }
      }

      return true;
   }
}

