#include "TreeReader.h"

#include <iostream>

using namespace std;
using namespace TreeReader;

static TreeFilterPtr CreateFilter(const wstring& filterText, bool useV1)
{
   TreeFilterPtr filter = useV1 ? ConvertTextToFilters(filterText) : ConvertSimpleTextToFilters(filterText);
   if (filter)
      return filter;

   wcerr << L"Invalid filter: " << filterText << std::endl;
   return {};
}

int wmain(int argc, wchar_t** argv)
{
   wstring file;
   bool useV1 = false;
   bool isInteractive = false;
   wstring filterText;

   for (int i = 1; i < argc; ++i)
   {
      const wstring arg = argv[i];
      if (arg == L"-v1" || arg == L"--v1")
      {
         useV1 = true;
      }
      else if (arg == L"-i" || arg == L"--interactive")
      {
         isInteractive = true;
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

   if (file.empty() || (!isInteractive && filterText.empty()))
   {
      wcerr << L"Usage: " << argv[0] << L" <tree-file> <filter-description>" << std::endl;
      return 1;
   }

   TreeFilterPtr filter;
   if (!filterText.empty())
   {
      filter = CreateFilter(filterText, useV1);
      if (!filter)
      {
         return 1;
      }
   }

   TextTree tree = ReadSimpleTextTree(filesystem::path(file));

   TextTree filtered;
   while (true)
   {
      if (filter)
      {
         FilterTree(tree, filtered, filter);
         wcout << filtered << endl;
      }

      if (!isInteractive)
         break;

      wcout << L"New filter: "; 
      wchar_t buffer[256];
      wcin.getline(buffer, sizeof(buffer)/sizeof(buffer[0]));
      if (!wcin)
         break;
      filterText = buffer;
      filter = CreateFilter(filterText, useV1);
   }
}
