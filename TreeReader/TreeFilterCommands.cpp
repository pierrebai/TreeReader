#include "TreeFilterCommands.h"
#include "TreeCommands.h"
#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"
#include "SimpleTreeWriter.h"

#include <sstream>
#include <fstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Context constructor.
   CommandsContext::CommandsContext()
   {
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree loading and saving.

   void CommandsContext::SetInputFilter(const wstring& filterRegex)
   {
      Options.ReadOptions.InputFilter = filterRegex;
   }

   void CommandsContext::SetInputIndent(const wstring& indentText)
   {
      Options.ReadOptions.InputIndent = indentText;
   }

   void CommandsContext::SetOutputIndent(const wstring& indentText)
   {
      Options.OutputLineIndent = indentText;
   }

   TreeCommandsPtr CommandsContext::LoadTree(const filesystem::path& filename)
   {
      auto newTree = make_shared<TextTree>(ReadSimpleTextTree(filename, Options.ReadOptions));
      if (newTree && newTree->Roots.size() > 0)
      {
         auto treeCmd = make_shared<TreeCommands>(newTree, filename, _knownFilters, _undoRedo);
         _trees.emplace_back(treeCmd);
         return treeCmd;
      }
      else
      {
         return {};
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Named filters management.

   NamedFilterPtr CommandsContext::NameFilter(const wstring& filterName, const TreeCommandsPtr& tree)
   {
      tree->SetFilterName(filterName);
      return NameFilter(filterName, tree->GetFilter());
   }

   NamedFilterPtr CommandsContext::NameFilter(const wstring& filterName, const TreeFilterPtr& filter)
   {
      return _knownFilters->Add(filterName, filter);
   }

   bool CommandsContext::RemoveNamedFilter(const wstring& filterName)
   {
      return _knownFilters->Remove(filterName);
   }

   void CommandsContext::SaveNamedFilters(const filesystem::path& filename)
   {
      if (_knownFilters->All().size() > 0)
         WriteNamedFilters(filename, *_knownFilters);
   }

   void CommandsContext::LoadNamedFilters(const filesystem::path& filename)
   {
      auto filters = ReadNamedFilters(filename);
      _knownFilters->Merge(filters);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Options.

   void CommandsContext::SaveOptions(const filesystem::path& filename)
   {
      wofstream stream(filename);
      SaveOptions(stream);
   }

   void CommandsContext::SaveOptions(std::wostream& stream)
   {
      stream << L"V1: " << L"\n"
      << L"output-indent: "   << quoted(Options.OutputLineIndent) << L"\n"
      << L"input-filter: "    << quoted(Options.ReadOptions.InputFilter) << L"\n"
      << L"input-indent: "    << quoted(Options.ReadOptions.InputIndent) << L"\n"
      << L"tab-size: "        << Options.ReadOptions.TabSize << L"\n";
   }

   void CommandsContext::LoadOptions(const filesystem::path& filename)
   {
      wifstream stream(filename);
      LoadOptions(stream);
   }

   void CommandsContext::LoadOptions(std::wistream& stream)
   {
      wstring v1;
      stream >> v1;
      if (v1 != L"V1:")
         return;

      while (stream)
      {
         wstring item;
         stream >> skipws >> item >> skipws;
         if (item == L"output-indent:")
         {
            stream >> quoted(Options.OutputLineIndent);

         }
         else if (item == L"input-filter:")
         {
            stream >> quoted(Options.ReadOptions.InputFilter);

         }
         else if (item == L"input-indent:")
         {
            stream >> quoted(Options.ReadOptions.InputIndent);

         }
         else if (item == L"tab-size:")
         {
            stream >> Options.ReadOptions.TabSize;

         }
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current text tree.

   void CommandsContext::RemoveTree(const TreeCommandsPtr& tree)
   {
      const auto pos = find(_trees.begin(), _trees.end(), tree);
      if (pos == _trees.end())
         return;

      _trees.erase(pos);
   }

   TreeCommandsPtr CommandsContext::CreateTreeFromFiltered(const TreeCommandsPtr& tree)
   {
      if (!tree)
         return {};

      auto newCtx = make_shared<TreeCommands>(tree->GetFilteredTree(), tree->GetFilteredTreeFileName(), _knownFilters, _undoRedo);

      _trees.emplace_back(newCtx);

      return newCtx;
   }

   void CommandsContext::ClearUndoStack()
   {
      _undoRedo->Clear();
   }

}
