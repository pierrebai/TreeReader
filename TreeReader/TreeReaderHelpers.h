#pragma once

#include <string>
#include <vector>
#include <functional>

namespace TreeReader
{
   enum class SplitOptions
   {
      KeepEmpty, RemoveEmpty
   };

   // Split text into parts using the given delimiter.
   std::vector<std::wstring> Split(const std::wstring& text, wchar_t delimiter = L' ', SplitOptions options = SplitOptions::RemoveEmpty);

   // Join multiple text parts into one using the given delimiter.
   std::wstring Join(const std::vector<std::wstring>& parts, wchar_t delimiter = L' ');

   // Call the function and catch all exceptions.
   void WithNoExceptions(const std::function<void()>& func);
}
