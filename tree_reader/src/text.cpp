#include "dak/utility/text.h"

#include <sstream>
#include <functional>

namespace dak::utility
{
   using namespace std;

   vector<wstring> split(const wstring& text, wchar_t delimiter, split_options options)
   {
      vector<wstring> result;
      wistringstream sstream(text);
      wstring item;
      while (getline(sstream, item, delimiter))
         if (!item.empty() || options == split_options::keep_empty)
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

   void with_no_exceptions(const std::function<void()>& func)
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
