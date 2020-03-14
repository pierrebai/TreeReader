#pragma once

#include "TextTree.h"

namespace TreeReader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Holds text as a vector of text strings.
   //
   // Beware that the string text pointers could theoretically moved while the vector
   // is filled, so don't rely on their address until all the lines have been read.

   struct TextLinesTextHolder : TextHolder
   {
      typedef std::vector<std::wstring> TextLines;

      TextLines Lines;
   };
}
