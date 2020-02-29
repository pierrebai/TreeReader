#pragma once

#include "TreeFilter.h"

namespace TreeReader
{
   const wchar_t* GetFilterName(const TreeFilter& filter);
   const wchar_t* GetFilterDescription(const TreeFilter& filter);

   // A visitor that delegates to a function when visiting each filter.

   typedef std::function<bool(TreeFilter* filter)> FilterVisitFunction;
   bool VisitFilters(TreeFilter* filter, FilterVisitFunction func);
}

