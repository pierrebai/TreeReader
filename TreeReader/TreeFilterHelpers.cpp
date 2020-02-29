#include "TreeFilterHelpers.h"

namespace TreeReader
{
   using namespace std;

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

