#pragma once

#include <string>
#include <vector>

namespace dak::utility
{
   enum class split_options
   {
      keep_empty, remove_empty
   };

   // split text into parts using the given delimiter.
   std::vector<std::wstring> split(const std::wstring& text, wchar_t delimiter = L' ', split_options options = split_options::remove_empty);

   // join multiple text parts into one using the given delimiter.
   std::wstring join(const std::vector<std::wstring>& parts, wchar_t delimiter = L' ');
}
