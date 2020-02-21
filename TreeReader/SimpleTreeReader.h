#pragma once

#include "TextTree.h"

#include <filesystem>
#include <regex>

namespace TreeReader
{
   struct ReadSimpleTextTreeOptions
   {
      size_t TabSize = 8;
      bool SimpleIndent = true;
      std::wregex IndentRegex = std::wregex(L"^[ \t]*");
   };

   TextTree ReadSimpleTextTree(const std::filesystem::path& path, const ReadSimpleTextTreeOptions& options = ReadSimpleTextTreeOptions());
   TextTree ReadSimpleTextTree(std::wistream& stream, const ReadSimpleTextTreeOptions& options = ReadSimpleTextTreeOptions());
}
