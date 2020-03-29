#include "TreeReaderHelpers.h"

#include <sstream>

namespace TreeReader
{
   using namespace std;

   vector<wstring> Split(const wstring& text, wchar_t delimiter, SplitOptions options)
   {
      vector<wstring> result;
      wistringstream sstream(text);
      wstring item;
      while (getline(sstream, item, delimiter))
         if (!item.empty() || options == SplitOptions::KeepEmpty)
            result.emplace_back(item);
      return result;
   }

   wstring Join(const vector<wstring>& parts, wchar_t delimiter)
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

   void WithNoExceptions(const std::function<void()>& func)
   {
      try
      {
         func();
      }
      catch (const exception&)
      {
         // Do nothing.
      }
   }
}
