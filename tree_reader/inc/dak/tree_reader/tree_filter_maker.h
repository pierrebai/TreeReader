#pragma once

#include "dak/tree_reader/tree_filter.h"
#include "dak/tree_reader/named_filters.h"

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Conversion from text to filters.

   // Create a textual description of a tree of filters.
   std::wstring  convert_filter_to_text(const tree_filter_ptr& filter);

   // Recreate a tree of filters from its textual description.
   tree_filter_ptr convert_text_to_filter(const std::wstring& text, const named_filters& named);

   // Create a tree of filters from simpler textual form.
   tree_filter_ptr convert_simple_text_to_filter(const std::wstring& text, const named_filters& named);
}
