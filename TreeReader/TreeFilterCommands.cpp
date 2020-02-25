#include "TreeFilterCommands.h"
#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"

#include <sstream>

namespace TreeReader
{
   using namespace std;

   static wstring GetHelp()
   {
      wostringstream stream;

      stream << L"Commands:" << endl;
      stream << L"  v1: use v1 text-to-filter conversion." << endl;
      stream << L"  no-v1: use the simple text-to-filter conversion." << endl;
      stream << L"  interactive: use an interactive prompt to enter ctx.Options, file name or filters." << endl;
      stream << L"  no-interactive: turn off the interactive mode." << endl;
      stream << L"  help: print this help." << endl;
      stream << L"  debug: print debug information while processing." << endl;
      stream << L"  no-debug: turn off debugging." << endl;
      stream << L"  input-filter ''regex'': a regular expression to filter input lines." << endl;
      stream << L"  input-indent ''text'': the characters detected as indentation in each line." << endl;
      stream << L"  output-indent ''text'': text to indent the printed lines." << endl;
      stream << L"  load ''file name'': the name of the file to read and filter." << endl;
      stream << L"  filter ''filter'': provide the text to be converted into filters." << endl;

      return stream.str();
   }

   static wstring CreateFilter(const wstring& filterText, CommandsContext& ctx)
   {
      wostringstream stream;

      ctx.Filter = ctx.Options.UseV1 ? ConvertTextToFilters(filterText) : ConvertSimpleTextToFilters(filterText);

      if (ctx.Options.Debug)
      {
         if (ctx.Filter)
            stream << L"Filters: " << ConvertFiltersToText(ctx.Filter) << endl;
         else
            stream << L"Invalid filter: " << filterText << endl;
      }
      return stream.str();
   }

   wstring ParseCommands(const wstring& cmdText, CommandsContext& ctx)
   {
      return ParseCommands(split(cmdText), ctx);
   }

   wstring ParseCommands(const vector<wstring>& cmds, CommandsContext& ctx)
   {
      wstring result;

      const CommandsContext previousCtx = ctx;
      ctx.FilterText = L"";

      for (size_t i = 0; i < cmds.size(); ++i)
      {
         const wstring& cmd = cmds[i];
         if (cmd == L"v1")
         {
            ctx.Options.UseV1 = true;
         }
         if (cmd == L"no-v1")
         {
            ctx.Options.UseV1 = false;
         }
         else if (cmd == L"interactive")
         {
            ctx.Options.IsInteractive = true;
         }
         else if (cmd == L"no-interactive")
         {
            ctx.Options.IsInteractive = false;
         }
         else if (cmd == L"help")
         {
            result += GetHelp();
         }
         else if (cmd == L"-d" || cmd == L"debug")
         {
            ctx.Options.Debug = true;
         }
         else if (cmd == L"no-debug")
         {
            ctx.Options.Debug = false;
         }
         else if (cmd == L"input-filter" && i + 1 < cmds.size())
         {
            ctx.Options.ReadOptions.InputFilter = cmds[++i];
         }
         else if (cmd == L"input-indent" && i + 1 < cmds.size())
         {
            ctx.Options.ReadOptions.InputIndent = cmds[++i];
         }
         else if (cmd == L"output-indent" && i + 1 < cmds.size())
         {
            ctx.Options.OutputLineIndent = cmds[++i];
         }
         else if (cmd == L"load" && i + 1 < cmds.size())
         {
            ctx.TreeFileName = cmds[++i];
         }
         else if (cmd == L"filter" && i + 1 < cmds.size())
         {
            if (!ctx.FilterText.empty())
               ctx.FilterText += L' ';
            ctx.FilterText += cmds[++i];
         }
         else
         {
            if (!ctx.FilterText.empty())
               ctx.FilterText += L' ';
            ctx.FilterText += cmd;
         }
      }

      if (ctx.FilterText.empty())
         ctx.FilterText = previousCtx.FilterText;

      const bool optionsChanged = (previousCtx.Options != ctx.Options);
      const bool readOptionsChanged = (previousCtx.Options.ReadOptions != ctx.Options.ReadOptions);
      const bool fileChanged = (previousCtx.TreeFileName != ctx.TreeFileName);
      const bool filterChanged = (previousCtx.FilterText != ctx.FilterText);

      if (fileChanged || readOptionsChanged)
         ctx.Tree = make_shared<TextTree>(ReadSimpleTextTree(filesystem::path(ctx.TreeFileName), ctx.Options.ReadOptions));

      if (filterChanged || optionsChanged)
         result += CreateFilter(ctx.FilterText, ctx);

      if (ctx.Filter && ctx.Tree && (fileChanged || filterChanged || optionsChanged || readOptionsChanged))
      {
         ctx.Filtered = make_shared<TextTree>();
         FilterTree(*ctx.Tree, *ctx.Filtered, ctx.Filter);
      }

      return result;
   }
}