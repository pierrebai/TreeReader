#include <string>
#include <vector>

namespace TreeReader
{
   enum class SplitOptions
   {
      KeepEmpty, RemoveEmpty
   };

   std::vector<std::wstring> split(const std::wstring& text, wchar_t delimiter = L' ', SplitOptions options = SplitOptions::RemoveEmpty);
   std::wstring join(const std::vector<std::wstring>& parts, wchar_t delimiter = L' ');
}
