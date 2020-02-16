#include "TextFilter.h"

namespace TreeReader
{
   bool NotTextFilter::IsKept(const Text& text)
   {
      return !Filter || !Filter->IsKept(text);
   }

   bool OrTextFilter::IsKept(const Text& text)
   {
      for (const auto& filter : Filters)
         if (filter && filter->IsKept(text))
            return true;
      return false;
   }

   bool AndTextFilter::IsKept(const Text& text)
   {
      for (const auto& filter : Filters)
         if (filter && !filter->IsKept(text))
            return false;
      return true;
   }
}
