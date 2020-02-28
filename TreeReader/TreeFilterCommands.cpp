#include "TreeFilterCommands.h"
#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"
#include "SimpleTreeWriter.h"

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
      stream << L"  interactive: use an interactive prompt to enter options, file name or filters." << endl;
      stream << L"  no-interactive: turn off the interactive mode." << endl;
      stream << L"  help: print this help." << endl;
      stream << L"  debug: print debug information while processing other commands." << endl;
      stream << L"  no-debug: turn off debugging." << endl;
      stream << L"  input-filter ''regex'': filter input lines using the given regular expression." << endl;
      stream << L"  input-indent ''text'': detect the indentation of each line using the given characters." << endl;
      stream << L"  output-indent ''text'': indent the printed lines with the given text." << endl;
      stream << L"  load ''file name'': load a text tree from the given file." << endl;
      stream << L"       (The tree is pushed on the active tree stack, ready to be filtered.)" << endl;
      stream << L"  save ''file name'': save the tree into the named file." << endl;
      stream << L"  filter ''filter'': convert the given textual filters description into filters." << endl;
      stream << L"  push-filtered: use the current filtered tree as input to the filters." << endl;
      stream << L"  pop-tree: pop the current tree and use the previous tree as input to the filters." << endl;
      stream << L"  then: apply the filters immediately, push the result as being the current tree and starts new filters." << endl;
      stream << L"  name ''name'': give a name to the current filter." << endl;
      stream << L"  save-filters ''file name'': save all named filters to the given file." << endl;
      stream << L"  load-filters ''file name'': load named filters from the given file." << endl;
      stream << L"  list-filters: list all the named filters." << endl;

      return stream.str();
   }

   static wstring CreateFilter(const wstring& filterText, CommandsContext& ctx)
   {
      wostringstream stream;

      ctx.Filter = ctx.Options.UseV1 ? ConvertTextToFilters(filterText,ctx.NamedFilters) : ConvertSimpleTextToFilters(filterText, ctx.NamedFilters);

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

      CommandsContext previousCtx = ctx;
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
            auto newTree = make_shared<TextTree>(ReadSimpleTextTree(filesystem::path(ctx.TreeFileName), ctx.Options.ReadOptions));
            if (newTree && newTree->Roots.size() > 0)
            {
               ctx.Trees.emplace_back(move(newTree));
            }
            else
            {
               result += L"Tree file was invalid or empty.\n";
            }
         }
         else if (cmd == L"save" && i + 1 < cmds.size())
         {
            ctx.TreeFileName = cmds[++i];
            if (ctx.Trees.size() > 0 && ctx.Trees.back())
               WriteSimpleTextTree(filesystem::path(ctx.TreeFileName), *ctx.Trees.back(), ctx.Options.OutputLineIndent);
         }
         else if (cmd == L"filter" && i + 1 < cmds.size())
         {
            if (!ctx.FilterText.empty())
               ctx.FilterText += L' ';
            ctx.FilterText += cmds[++i];
         }
         else if (cmd == L"push-filtered" && ctx.Filtered)
         {
            ctx.Trees.emplace_back(move(ctx.Filtered));
            ctx.Filtered = nullptr;
         }
         else if (cmd == L"pop-tree")
         {
            if (ctx.Trees.size() > 0)
               ctx.Trees.pop_back();
            ctx.Filtered = nullptr;
         }
         else if (cmd == L"then")
         {
            if (!ctx.FilterText.empty())
               result += CreateFilter(ctx.FilterText, ctx);
            if (ctx.Filter && ctx.Trees.size() > 0)
            {
               ctx.Filtered = make_shared<TextTree>();
               FilterTree(*ctx.Trees.back(), *ctx.Filtered, ctx.Filter);
               ctx.Trees.emplace_back(move(ctx.Filtered));
               ctx.Filtered = nullptr;
            }
            ctx.FilterText = L"";
            previousCtx.FilterText = L"";
         }
         else if (cmd == L"name" && i + 1 < cmds.size())
         {
            if (!ctx.FilterText.empty())
               result += CreateFilter(ctx.FilterText, ctx);
            const wstring name = cmds[++i];
            if (ctx.Filter)
               ctx.NamedFilters.Filters[name] = ctx.Filter;
         }
         else if (cmd == L"save-filters" && i + 1 < cmds.size())
         {
            if (ctx.NamedFilters.Filters.size() > 0)
            {
               const wstring filename = cmds[++i];
               WriteNamedFilters(filename, ctx.NamedFilters);
            }
         }
         else if (cmd == L"load-filters" && i + 1 < cmds.size())
         {
            const wstring filename = cmds[++i];
            auto filters = ReadNamedFilters(filename);
            ctx.NamedFilters.Filters.insert(filters.Filters.begin(), filters.Filters.end());
         }
         else if (cmd == L"list-filters")
         {
            wostringstream sstream;
            for (const auto& [name, filter] : ctx.NamedFilters.Filters)
            {
               sstream << name << endl;
            }
            result += sstream.str();
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
      const bool filterTextChanged = (previousCtx.FilterText != ctx.FilterText || previousCtx.Options.UseV1 != ctx.Options.UseV1);
      const bool filterChanged = (filterTextChanged || previousCtx.Filter != ctx.Filter);
      const bool treeChanged = (previousCtx.Trees.size() != ctx.Trees.size() || (previousCtx.Trees.size() > 0 && previousCtx.Trees.back() != ctx.Trees.back()));

      if (filterTextChanged)
         result += CreateFilter(ctx.FilterText, ctx);

      if (ctx.Filter && ctx.Trees.size() > 0 && (fileChanged || filterChanged || optionsChanged || readOptionsChanged || treeChanged))
      {
         ctx.Filtered = make_shared<TextTree>();
         FilterTree(*ctx.Trees.back(), *ctx.Filtered, ctx.Filter);
      }

      return result;
   }
}
