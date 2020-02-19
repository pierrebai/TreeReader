#pragma once

#include "TreeFilter.h"

namespace TreeReader
{
   std::wstring  ConvertFiltersToText(const TreeFilterPtr& filter);
   TreeFilterPtr ConvertTextToFilters(const std::wstring& text);

   TreeFilterPtr ConvertSimpleTextToFilters(const std::wstring& text);
}
