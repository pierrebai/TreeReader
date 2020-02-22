#include "TreeReaderHelpers.h"

#include <sstream>

namespace TreeReader
{
   using namespace std;

   vector<wstring> split(const wstring& text, wchar_t delimiter)
   {
      vector<wstring> result;
      wistringstream sstream(text);
      wstring item;
      while (getline(sstream, item, delimiter))
         result.emplace_back(item);
      return result;
   }

   wstring join(const vector<wstring>& parts, wchar_t delimiter)
   {
      wstring result;

      auto pos = parts.begin();
      const auto end = parts.end();
      if (pos != end)
      {
         result += *pos++;
      }

      while (pos != end)
      {
         result += delimiter;
         result += *pos++;
      }

      return result;
   }
}
