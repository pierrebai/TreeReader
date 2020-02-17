#pragma once

#include "TextTree.h"

#include <filesystem>

namespace TreeReader
{
   struct ReadSimpleTextTreeOptions
   {
      size_t TabSize = 8;
   };

   TextTree ReadSimpleTextTree(const std::filesystem::path& path, const ReadSimpleTextTreeOptions& options = ReadSimpleTextTreeOptions());
   TextTree ReadSimpleTextTree(std::wistream& stream, const ReadSimpleTextTreeOptions& options = ReadSimpleTextTreeOptions());
}
