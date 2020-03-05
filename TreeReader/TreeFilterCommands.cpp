#include "TreeFilterCommands.h"
#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"
#include "SimpleTreeWriter.h"

#include <sstream>

namespace TreeReader
{
   using namespace std;

   wstring CommandsContext::GetHelp() const
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

   void CommandsContext::SetInputFilter(const std::wstring& filterRegex)
   {
      Options.ReadOptions.InputFilter = filterRegex;
   }

   void CommandsContext::SetInputIndent(const std::wstring& indentText)
   {
      Options.ReadOptions.InputIndent = indentText;
   }

   void CommandsContext::SetOutputIndent(const std::wstring& indentText)
   {
      Options.OutputLineIndent = indentText;
   }

   wstring CommandsContext::LoadTree(const filesystem::path& filename)
   {
      TreeFileName = filename;
      auto newTree = make_shared<TextTree>(ReadSimpleTextTree(filesystem::path(TreeFileName), Options.ReadOptions));
      if (newTree && newTree->Roots.size() > 0)
      {
         Trees.emplace_back(move(newTree));
         return {};
      }
      else
      {
         return L"Tree file was invalid or empty.\n";
      }
   }

   void CommandsContext::SaveFilteredTree(const filesystem::path& filename)
   {
      FilteredFileName = filename;
      if (Filtered)
      {
         WriteSimpleTextTree(filesystem::path(FilteredFileName), *Filtered, Options.OutputLineIndent);
         FilteredWasSaved = true;
      }
   }

   void CommandsContext::AppendFilterText(const std::wstring& text)
   {
      if (!FilterText.empty())
         FilterText += L' ';
      FilterText += text;
   }

   void CommandsContext::ClearFilterText()
   {
      FilterText = L"";
   }

   wstring CommandsContext::CreateFilter()
   {
      return CreateFilter(FilterText);
   }

   wstring CommandsContext::CreateFilter(const wstring& filterText)
   {
      wostringstream stream;

      Filter = Options.UseV1
             ? ConvertTextToFilters(filterText, *KnownFilters)
             : ConvertSimpleTextToFilters(filterText, *KnownFilters);

      if (Options.Debug)
      {
         if (Filter)
            stream << L"Filters: " << ConvertFiltersToText(Filter) << endl;
         else
            stream << L"Invalid filter: " << filterText << endl;
      }
      return stream.str();
   }

   void CommandsContext::NameFilter(const std::wstring& filterName)
   {
      NameFilter(filterName, Filter);
   }

   void CommandsContext::NameFilter(const std::wstring& filterName, const TreeFilterPtr& filter)
   {
      KnownFilters->Add(filterName, filter);
   }

   wstring CommandsContext::ListNamedFilters()
   {
      wostringstream sstream;
      for (const auto& [name, filter] : KnownFilters->All())
         sstream << name << endl;
      return sstream.str();
   }

   void CommandsContext::SaveNamedFilters(const std::filesystem::path& filename)
   {
      if (KnownFilters->All().size() > 0)
         WriteNamedFilters(filename, *KnownFilters);
   }

   void CommandsContext::LoadNamedFilters(const std::filesystem::path& filename)
   {
      auto filters = ReadNamedFilters(filename);
      KnownFilters->Merge(filters);
   }

   void CommandsContext::ApplyFilterToTree()
   {
      if (Trees.size() <= 0)
         return;

      if (Filter)
      {
         Filtered = make_shared<TextTree>();
         FilterTree(*Trees.back(), *Filtered, Filter);
         FilteredWasSaved = false;
      }
      else
      {
         Filtered = make_shared<TextTree>(*Trees.back());
         // Note: pure copy of input tree are considered to have been saved.
         FilteredWasSaved = true;
      }
   }

   void CommandsContext::PushFilteredAsTree()
   {
      if (Filtered)
      {
         Trees.emplace_back(move(Filtered));
      }
   }
   
   void CommandsContext::PopTree()
   {
      if (Trees.size() > 0)
         Trees.pop_back();
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
            result += ctx.GetHelp();
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
            ctx.SetInputFilter(cmds[++i]);
         }
         else if (cmd == L"input-indent" && i + 1 < cmds.size())
         {
            ctx.SetInputIndent(cmds[++i]);
         }
         else if (cmd == L"output-indent" && i + 1 < cmds.size())
         {
            ctx.SetOutputIndent(cmds[++i]);
         }
         else if (cmd == L"load" && i + 1 < cmds.size())
         {
            result += ctx.LoadTree(cmds[++i]);
         }
         else if (cmd == L"save" && i + 1 < cmds.size())
         {
            ctx.SaveFilteredTree(cmds[++i]);
         }
         else if (cmd == L"filter" && i + 1 < cmds.size())
         {
            ctx.AppendFilterText(cmds[++i]);
         }
         else if (cmd == L"push-filtered")
         {
            ctx.PushFilteredAsTree();
         }
         else if (cmd == L"pop-tree")
         {
            ctx.PopTree();
         }
         else if (cmd == L"then")
         {
            result += ctx.CreateFilter();
            ctx.ApplyFilterToTree();
            ctx.PushFilteredAsTree();
            ctx.ClearFilterText();
            previousCtx.FilterText = L"";
         }
         else if (cmd == L"name" && i + 1 < cmds.size())
         {
            result += ctx.CreateFilter();
            ctx.NameFilter(cmds[++i]);
         }
         else if (cmd == L"save-filters" && i + 1 < cmds.size())
         {
            ctx.SaveNamedFilters(cmds[++i]);
         }
         else if (cmd == L"load-filters" && i + 1 < cmds.size())
         {
            ctx.LoadNamedFilters(cmds[++i]);
         }
         else if (cmd == L"list-filters")
         {
            result += ctx.ListNamedFilters();
         }
         else
         {
            ctx.AppendFilterText(cmd);
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
         result += ctx.CreateFilter();

      if (fileChanged || filterChanged || optionsChanged || readOptionsChanged || treeChanged)
      {
         ctx.ApplyFilterToTree();
      }

      return result;
   }
}
