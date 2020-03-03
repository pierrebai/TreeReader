#pragma once

#include "TreeFilter.h"

namespace TreeReader
{
   // A visitor that delegates to a function when visiting each filter.
   //
   // Allows starting from an arbitrary filter and not visiting the initial filter.
   //
   // (Not visiting siblings also skip the initial node too.)

   typedef std::function<bool(const TreeFilterPtr& filter)> FilterVisitFunction;

   bool VisitFilters(const TreeFilterPtr& filter, bool includeSelf, FilterVisitFunction func);

   inline bool VisitFilters(const TreeFilterPtr& filter, FilterVisitFunction func)
   {
      return VisitFilters(filter, true, func);
   }
}

