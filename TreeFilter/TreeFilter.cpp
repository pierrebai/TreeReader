#include "TreeReader.h"

#include <iostream>

using namespace std;
using namespace TreeReader;

struct Options
{
   bool UseV1 = false;
   bool IsInteractive = false;
   bool Debug = false;
   wstring OutputLineIndent = L"  ";
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

static void HandleOptions(vector<wstring> args, wstring& file, wstring& filterText, Options& options, ReadSimpleTextTreeOptions& readOptions)
{
   for (size_t i = 0; i < args.size(); ++i)
   {
      const wstring& arg = args[i];
      if (arg == L"-v1" || arg == L"--v1")
      {
         options.UseV1 = true;
      }
      if (arg == L"-v1" || arg == L"--no-v1")
      {
         options.UseV1 = false;
      }
      else if (arg == L"-i" || arg == L"--interactive")
      {
         options.IsInteractive = true;
      }
      else if (arg == L"-i" || arg == L"--no-interactive")
      {
         options.IsInteractive = false;
      }
      else if (arg == L"-d" || arg == L"--debug")
      {
         options.Debug = true;
      }
      else if (arg == L"--no-debug")
      {
         options.Debug = false;
      }
      else if (arg == L"--input-filter" && i + 1 < args.size())
      {
         readOptions.FilterInput = true;
         readOptions.InputFilter = args[++i];
      }
      else if (arg == L"--input-indent" && i + 1 < args.size())
      {
         readOptions.InputIndent = args[++i];
      }
      else if (arg == L"--output-indent" && i + 1 < args.size())
      {
         options.OutputLineIndent = args[++i];
      }
      else if (arg == L"--file" && i + 1 < args.size())
      {
         file = args[++i];
      }
      else if (arg == L"--filter" && i + 1 < args.size())
      {
         if (!filterText.empty())
            filterText += L' ';
         filterText += args[++i];
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
}

int wmain(int argc, wchar_t** argv)
{
   Options options;
   ReadSimpleTextTreeOptions readOptions;

   TextTree tree;
   TextTree filtered;
   TreeFilterPtr filter;

   wstring previousFile;
   wstring previousFilterText;

   vector<wstring> args(argv + min(1, argc), argv + argc);

   while (true)
   {
      wstring file = previousFile;
      wstring filterText;
      HandleOptions(args, file, filterText, options, readOptions);

      if (!options.IsInteractive)
      {
         if (file.empty() || filterText.empty())
         {
            wcerr << L"Usage: " << argv[0] << L" [options] <tree-file> <filter-description>" << std::endl;
            return 1;
         }
      }

      const bool fileChanged = (file != previousFile);
      if (fileChanged)
      {
         previousFile = file;
         tree = ReadSimpleTextTree(filesystem::path(file), readOptions);
      }

      const bool filterChanged = (filterText != previousFilterText);
      if (filterChanged)
      {
         previousFilterText = filterText;
         filter = CreateFilter(filterText, options);
      }

      if (filter && (fileChanged || filterChanged))
      {
         FilterTree(tree, filtered, filter);
         PrintTree(wcout, filtered, options.OutputLineIndent) << endl;
      }

      if (!options.IsInteractive)
         break;

      wcout << L"New arguments: "; 
      wchar_t buffer[256];
      wcin.getline(buffer, sizeof(buffer)/sizeof(buffer[0]));
      if (!wcin)
         break;
      args = split(buffer);
   }
}
