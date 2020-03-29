#pragma once

#include "dak/tree_reader/text_tree.h"

#include <filesystem>

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Write a simple flat text file, using initial white-space indentation
   // to determine the tree structure.

   void save_simple_text_tree(const std::filesystem::path& path, const text_tree& tree, const std::wstring& indentation);
   void save_simple_text_tree(std::wostream& stream, const text_tree& tree, const std::wstring& indentation);
}
