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

   bool operator!=(const Options& other) const
   {
      return UseV1 != other.UseV1
          || IsInteractive != other.IsInteractive
          || Debug != other.Debug
          || OutputLineIndent != other.OutputLineIndent;
   }
};

static TreeFilterPtr CreateFilter(const wstring& filterText, const Options& opt)
{
   TreeFilterPtr filter = opt.UseV1 ? ConvertTextToFilters(filterText) : ConvertSimpleTextToFilters(filterText);
   if (opt.Debug)
      wcout << L"Filters: " << ConvertFiltersToText(filter) << endl;
   if (filter)
      return filter;

   wcerr << L"Invalid filter: " << filterText << endl;
   return {};
}

static void PrintHelp(wostream& stream, wstring programName)
{
   stream << L"Usage: " << programName << L" [options] <tree-file> <filter-description>" << endl;
   stream << L"Options:" << endl;
   stream << L"  -v1 or --v1: use v1 text-to-filter conversion." << endl;
   stream << L"  --no-v1: use the simple text-to-filter conversion." << endl;
   stream << L"  -i  or --interactive: use an interactive prompt to enter options, file name or filters." << endl;
   stream << L"  --no-interactive: turn off the interactive mode." << endl;
   stream << L"  -h  or --help: print this help." << endl;
   stream << L"  -d  or --debug: print debug information while processing." << endl;
   stream << L"  --no-debug: turn off debugging." << endl;
   stream << L"  --input-filter ''regex'': a regular expression to filter input lines." << endl;
   stream << L"  --input-indent ''text'': the characters detected as indentation in each line." << endl;
   stream << L"  --output-indent ''text'': text to indent the printed lines." << endl;
   stream << L"  --file ''file name'': the name of the file to read and filter." << endl;
   stream << L"  --filter ''filter'': provide the text to be converted into filters." << endl;
}

static void HandleOptions(wstring programName, vector<wstring> args, wstring& file, wstring& filterText, Options& options, ReadSimpleTextTreeOptions& readOptions)
{
   for (size_t i = 0; i < args.size(); ++i)
   {
      const wstring& arg = args[i];
      if (arg == L"-v1" || arg == L"--v1")
      {
         options.UseV1 = true;
      }
      if (arg == L"--no-v1")
      {
         options.UseV1 = false;
      }
      else if (arg == L"-i" || arg == L"--interactive")
      {
         options.IsInteractive = true;
      }
      else if (arg == L"--no-interactive")
      {
         options.IsInteractive = false;
      }
      else if (arg == L"-h" || arg == L"--help")
      {
         PrintHelp(wcout, programName);
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
   TextTree tree;
   TextTree filtered;
   TreeFilterPtr filter;

   Options options;
   ReadSimpleTextTreeOptions readOptions;

   wstring file;
   wstring filterText;

   wstring programName = argc > 0 ? argv[0] : L"TreeFilter";
   vector<wstring> args(argv + min(1, argc), argv + argc);

   while (true)
   {
      bool optionsChanged = false;
      bool readOptionsChanged = false;
      bool fileChanged = false;
      bool filterChanged = false;
      {
         const Options previousOptions = options;
         const ReadSimpleTextTreeOptions previousReadOptions = readOptions;
         const wstring previousFile = file;
         const wstring previousFilterText = filterText;

         HandleOptions(programName, args, file, filterText, options, readOptions);

         optionsChanged = (previousOptions != options);
         readOptionsChanged = (previousReadOptions != readOptions);
         fileChanged = (previousFile != file);
         filterChanged = (previousFilterText != filterText);
      }

      if (!options.IsInteractive)
      {
         if (file.empty() || filterText.empty())
         {
            PrintHelp(wcerr, programName);
            return 1;
         }
      }

      if (fileChanged || readOptionsChanged)
      {
         tree = ReadSimpleTextTree(filesystem::path(file), readOptions);
      }

      if (filterChanged || optionsChanged)
      {
         filter = CreateFilter(filterText, options);
      }

      if (filter && (fileChanged || filterChanged || optionsChanged || readOptionsChanged))
      {
         FilterTree(tree, filtered, filter);
         PrintTree(wcout, filtered, options.OutputLineIndent) << endl;
      }

      if (!options.IsInteractive)
         break;

      wcout << L"New arguments: "; 
      wchar_t buffer[256];
      wcin.getline(buffer, sizeof(buffer)/sizeof(buffer[0]));
      if (!wcin || !buffer[0])
         break;
      args = split(buffer);
   }
}
