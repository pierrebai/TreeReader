#pragma once

#include "TextTree.h"

#include <filesystem>
#include <regex>

namespace TreeReader
{
   // Options controling how the text is read and its indentation calculated.

   struct ReadSimpleTextTreeOptions
   {
      size_t TabSize = 8;
      bool SimpleIndent = true;
      std::wregex IndentRegex = std::wregex(L"^[ \t]*");
   };

   // Read a simple flat text file, using initial whit-space indentation to determine the tree structure.

   TextTree ReadSimpleTextTree(const std::filesystem::path& path, const ReadSimpleTextTreeOptions& options = ReadSimpleTextTreeOptions());
   TextTree ReadSimpleTextTree(std::wistream& stream, const ReadSimpleTextTreeOptions& options = ReadSimpleTextTreeOptions());
}
