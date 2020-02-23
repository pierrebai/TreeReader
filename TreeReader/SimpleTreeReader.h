#pragma once

#include "TextTree.h"

#include <filesystem>
#include <regex>

namespace TreeReader
{
   // Options controling how the text is read and its indentation calculated.

   struct ReadSimpleTextTreeOptions
   {
      // How many characters a tab represent to calculate line indentation.
      size_t TabSize = 8;

      // Characters that are considered indentation for input lines.
      std::wstring InputIndent = L" \t";

      // Optional input regular expression to filter input lines.
      // It is applied before indentation calculations.
      // Only captured text will be kept for further processing.
      // This allows cleaning up input lines.
      std::wstring InputFilter;

      bool operator!=(const ReadSimpleTextTreeOptions& other) const
      {
         return TabSize != other.TabSize
             || InputIndent != other.InputIndent
             || InputFilter != other.InputFilter;

      }
   };

   // Read a simple flat text file, using initial whit-space indentation to determine the tree structure.

   TextTree ReadSimpleTextTree(const std::filesystem::path& path, const ReadSimpleTextTreeOptions& options = ReadSimpleTextTreeOptions());
   TextTree ReadSimpleTextTree(std::wistream& stream, const ReadSimpleTextTreeOptions& options = ReadSimpleTextTreeOptions());
}
