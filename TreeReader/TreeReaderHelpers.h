#include <string>
#include <vector>

namespace TreeReader
{
   std::vector<std::wstring> split(const std::wstring& text, wchar_t delimiter = L' ');
   std::wstring join(const std::vector<std::wstring>& parts, wchar_t delimiter = L' ');
}
