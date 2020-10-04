#pragma once

#include "dak/tree_reader/tree_filter.h"

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A visitor that delegates to a function when visiting each filter.
   //
   // allows starting from an arbitrary filter and not visiting the initial filter.
   //
   // (not visiting siblings also skip the initial node too.)

   typedef std::function<bool(const tree_filter_ptr_t& filter)> filter_visit_function_t;

   bool visit_filters(const tree_filter_ptr_t& filter, bool includeSelf, filter_visit_function_t func);

   inline bool visit_filters(const tree_filter_ptr_t& filter, filter_visit_function_t func)
   {
      return visit_filters(filter, true, func);
   }
}

