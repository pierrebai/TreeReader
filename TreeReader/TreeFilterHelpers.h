#pragma once

#include "TreeFilter.h"

namespace TreeReader
{
   // A visitor that delegates to a function when visiting each filter.
   //
   // Allows starting from an arbitrary filter and not visiting the initial filter.
   //
   // (Not visiting siblings also skip the initial node too.)

   typedef std::function<bool(TreeFilter* filter)> FilterVisitFunction;

   bool VisitFilters(TreeFilter* filter, bool includeSelf, FilterVisitFunction func);

   inline bool VisitFilters(TreeFilter* filter, FilterVisitFunction func)
   {
      return VisitFilters(filter, true, func);
   }
}

