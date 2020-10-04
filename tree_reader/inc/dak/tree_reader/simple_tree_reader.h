#pragma once

#include "dak/tree_reader/text_tree.h"

#include <filesystem>
#include <regex>

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // options controling how the text is read and its indentation calculated.

   struct load_simple_text_tree_options_t
   {
      // How many characters a tab represent to calculate line indentation.
      size_t tab_size = 8;

      // Characters that are considered indentation for input lines.
      std::wstring input_indent = L" \t";

      // Optional input regular expression to filter input lines.
      // It is applied before indentation calculations.
      // Only captured text will be kept for further processing.
      // This allows cleaning up input lines.
      std::wstring input_filter;

      bool operator!=(const load_simple_text_tree_options_t& other) const
      {
         return tab_size     != other.tab_size
             || input_indent != other.input_indent
             || input_filter != other.input_filter;

      }
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Read a simple flat text file, using initial white-space indentation
   // to determine the tree structure.

   text_tree_t load_simple_text_tree(const std::filesystem::path& path, const load_simple_text_tree_options_t& options = load_simple_text_tree_options_t());
   text_tree_t load_simple_text_tree(std::wistream& stream, const load_simple_text_tree_options_t& options = load_simple_text_tree_options_t());
}
