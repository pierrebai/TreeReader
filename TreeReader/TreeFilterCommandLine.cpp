#include "TreeFilterCommandLine.h"
#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"

#include <sstream>

namespace TreeReader
{
   using namespace std;

   wstring CommandLine::GetHelp() const
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

   void CommandLine::AppendFilterText(const std::wstring& text)
   {
      if (!FilterText.empty())
         FilterText += L' ';
      FilterText += text;
   }

   void CommandLine::ClearFilterText()
   {
      FilterText = L"";
   }

   wstring CommandLine::CreateFilter()
   {
      return CreateFilter(FilterText);
   }

   wstring CommandLine::CreateFilter(const wstring& filterText)
   {
      wostringstream stream;

      SetFilter(UseV1
               ? ConvertTextToFilters(filterText, *_knownFilters)
               : ConvertSimpleTextToFilters(filterText, *_knownFilters));

      if (Debug)
      {
         if (_filter)
            stream << L"Filters: " << ConvertFiltersToText(_filter) << endl;
         else
            stream << L"Invalid filter: " << filterText << endl;
      }
      return stream.str();
   }

   wstring CommandLine::ListNamedFilters()
   {
      wostringstream sstream;
      for (const auto& [name, filter] : _knownFilters->All())
         sstream << name << endl;
      return sstream.str();
   }

   wstring CommandLine::ParseCommands(const wstring& cmdText)
   {
      return ParseCommands(split(cmdText));
   }

   wstring CommandLine::ParseCommands(const vector<wstring>& cmds)
   {
      wstring result;

      CommandLine previousCtx = *this;
      FilterText = L"";

      for (size_t i = 0; i < cmds.size(); ++i)
      {
         const wstring& cmd = cmds[i];
         if (cmd == L"v1")
         {
            UseV1 = true;
         }
         if (cmd == L"no-v1")
         {
            UseV1 = false;
         }
         else if (cmd == L"interactive")
         {
            IsInteractive = true;
         }
         else if (cmd == L"no-interactive")
         {
            IsInteractive = false;
         }
         else if (cmd == L"help")
         {
            result += GetHelp();
         }
         else if (cmd == L"-d" || cmd == L"debug")
         {
            Debug = true;
         }
         else if (cmd == L"no-debug")
         {
            Debug = false;
         }
         else if (cmd == L"input-filter" && i + 1 < cmds.size())
         {
            SetInputFilter(cmds[++i]);
         }
         else if (cmd == L"input-indent" && i + 1 < cmds.size())
         {
            SetInputIndent(cmds[++i]);
         }
         else if (cmd == L"output-indent" && i + 1 < cmds.size())
         {
            SetOutputIndent(cmds[++i]);
         }
         else if (cmd == L"load" && i + 1 < cmds.size())
         {
            result += LoadTree(cmds[++i]);
         }
         else if (cmd == L"save" && i + 1 < cmds.size())
         {
            SaveFilteredTree(cmds[++i]);
         }
         else if (cmd == L"filter" && i + 1 < cmds.size())
         {
            AppendFilterText(cmds[++i]);
         }
         else if (cmd == L"push-filtered")
         {
            PushFilteredAsTree();
         }
         else if (cmd == L"pop-tree")
         {
            PopTree();
         }
         else if (cmd == L"then")
         {
            result += CreateFilter();
            ApplyFilterToTree();
            PushFilteredAsTree();
            ClearFilterText();
            previousCtx.FilterText = L"";
         }
         else if (cmd == L"name" && i + 1 < cmds.size())
         {
            result += CreateFilter();
            NameFilter(cmds[++i]);
         }
         else if (cmd == L"save-filters" && i + 1 < cmds.size())
         {
            SaveNamedFilters(cmds[++i]);
         }
         else if (cmd == L"load-filters" && i + 1 < cmds.size())
         {
            LoadNamedFilters(cmds[++i]);
         }
         else if (cmd == L"list-filters")
         {
            result += ListNamedFilters();
         }
         else
         {
            AppendFilterText(cmd);
         }
      }

      if (FilterText.empty())
         FilterText = previousCtx.FilterText;

      const bool optionsChanged = (previousCtx.Options != Options);
      const bool readOptionsChanged = (previousCtx.Options.ReadOptions != Options.ReadOptions);
      const bool fileChanged = (previousCtx._treeFileName != _treeFileName);
      const bool filterTextChanged = (previousCtx.FilterText != FilterText || previousCtx.UseV1 != UseV1);
      const bool filterChanged = (filterTextChanged || previousCtx._filter != _filter);
      const bool treeChanged = (previousCtx._trees.size() != _trees.size() || (previousCtx._trees.size() > 0 && previousCtx._trees.back() != _trees.back()));

      if (filterTextChanged)
         result += CreateFilter();

      if (fileChanged || filterChanged || optionsChanged || readOptionsChanged || treeChanged)
      {
         ApplyFilterToTree();
      }

      return result;
   }
}
