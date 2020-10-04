#pragma once

#include "dak/tree_reader/text_tree.h"

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Holds text as a vector of text strings.
   //
   // Beware that the string text pointers could theoretically moved while the vector
   // is filled, so don't rely on their address until all the lines have been read.

   struct text_lines_text_holder_t : text_holder_t
   {
      typedef std::vector<std::wstring> text_lines_t;

      text_lines_t lines;
   };
}
