#include "TreeReader.h"

#include <iostream>

using namespace std;
using namespace TreeReader;

struct Options
{
   bool UseV1 = false;
   bool IsInteractive = false;
   bool Debug = false;
};

static TreeFilterPtr CreateFilter(const wstring& filterText, const Options& opt)
{
   TreeFilterPtr filter = opt.UseV1 ? ConvertTextToFilters(filterText) : ConvertSimpleTextToFilters(filterText);
   if (opt.Debug)
      wcout << L"Filters: " << ConvertFiltersToText(filter) << endl;
   if (filter)
      return filter;

   wcerr << L"Invalid filter: " << filterText << std::endl;
   return {};
}

int wmain(int argc, wchar_t** argv)
{
   Options options;
   wstring file;
   wstring filterText;

   for (int i = 1; i < argc; ++i)
   {
      const wstring arg = argv[i];
      if (arg == L"-v1" || arg == L"--v1")
      {
         options.UseV1 = true;
      }
      else if (arg == L"-i" || arg == L"--interactive")
      {
         options.IsInteractive = true;
      }
      else if (arg == L"-d" || arg == L"--debug")
      {
         options.Debug = true;
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

   if (file.empty() || (!options.IsInteractive && filterText.empty()))
   {
      wcerr << L"Usage: " << argv[0] << L" <tree-file> <filter-description>" << std::endl;
      return 1;
   }

   TreeFilterPtr filter;
   if (!filterText.empty())
   {
      filter = CreateFilter(filterText, options);
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

      if (!options.IsInteractive)
         break;

      wcout << L"New filter: "; 
      wchar_t buffer[256];
      wcin.getline(buffer, sizeof(buffer)/sizeof(buffer[0]));
      if (!wcin)
         break;
      filterText = buffer;
      filter = CreateFilter(filterText, options);
   }
}