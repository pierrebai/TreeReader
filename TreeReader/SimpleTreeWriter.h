#pragma once

#include "TextTree.h"

#include <filesystem>

namespace TreeReader
{
   // Write a simple flat text file, using initial white-space indentation to determine the tree structure.

   void WriteSimpleTextTree(const std::filesystem::path& path, const TextTree& tree, const std::wstring& indentation);
   void WriteSimpleTextTree(std::wostream& stream, const TextTree& tree, const std::wstring& indentation);
}
