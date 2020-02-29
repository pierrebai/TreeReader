#pragma once

#include "TreeFilter.h"

namespace TreeReader
{
   // A visitor that delegates to a function when visiting each filter.

   typedef std::function<bool(TreeFilter* filter)> FilterVisitFunction;
   bool VisitFilters(TreeFilter* filter, FilterVisitFunction func);
}

