#include "dak/tree_reader/tree_filter_helpers.h"

namespace dak::tree_reader
{
   using namespace std;

   bool visit_filters(const tree_filter_ptr_t& filter, bool includeSelf, filter_visit_function_t func)
   {
      if (!filter)
         return true;

      if (includeSelf && !func(filter))
         return false;

      if (auto delegate = dynamic_pointer_cast<delegate_tree_filter_t>(filter))
      {
         if (!visit_filters(delegate->sub_filter, func))
            return false;
      }
      else if (auto combined = dynamic_pointer_cast<combine_tree_filter_t>(filter))
      {
         for (auto& child : combined->filters)
         {
            if (!visit_filters(child, func))
               return false;
         }
      }

      return true;
   }
}

