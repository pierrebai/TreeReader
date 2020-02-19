#include "TreeReader.h"

#include <iostream>

using namespace std;
using namespace TreeReader;

int wmain(int argc, wchar_t** argv)
{
   wstring file;
   bool useV1 = false;
   wstring filterText;

   for (int i = 1; i < argc; ++i)
   {
      const wstring arg = argv[i];
      if (arg == L"-v1")
      {
         useV1 = true;
      }
      else if (file.empty())
      {
         file = arg;
      }
      else
      {
         if (!filterText.empty())
            filterText += L' ';
         filterText += arg;
      }
   }

   if (file.empty() || filterText.empty())
   {
      wcerr << L"Usage: " << argv[0] << L" <tree-file> <filter-description>" << std::endl;
      return 1;
   }

   TreeFilterPtr filter = useV1 ? ConvertTextToFilters(filterText) : ConvertSimpleTextToFilters(filterText);
   if (!filter)
   {
      wcerr << L"Invalid filter: " << filterText << std::endl;
      return 1;
   }

   TextTree tree = ReadSimpleTextTree(filesystem::path(file));

   TextTree filtered;
   FilterTree(tree, filtered, filter);

   wcout << filtered << endl;
}
