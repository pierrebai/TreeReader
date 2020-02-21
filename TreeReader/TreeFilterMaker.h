#pragma once

#include "TreeFilter.h"

namespace TreeReader
{
   // Create a textual description of a tree of filters.
   std::wstring  ConvertFiltersToText(const TreeFilterPtr& filter);

   // Recreate a tree of filters from its textual description.
   TreeFilterPtr ConvertTextToFilters(const std::wstring& text);

   // Create a tree of filters from simpler textual form.
   TreeFilterPtr ConvertSimpleTextToFilters(const std::wstring& text);
}
