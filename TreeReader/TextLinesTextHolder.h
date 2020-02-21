#pragma once

#include "TextTree.h"

namespace TreeReader
{
   struct TextLinesTextHolder : TextHolder
   {
      typedef std::vector<std::wstring> TextLines;

      TextLines Lines;
   };
}
