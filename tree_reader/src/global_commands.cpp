#include "GlobalCommands.h"
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
   GlobalCommands::GlobalCommands()
   {
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree loading and saving.

   void GlobalCommands::SetInputFilter(const wstring& filterRegex)
   {
      Options.ReadOptions.InputFilter = filterRegex;
   }

   void GlobalCommands::SetInputIndent(const wstring& indentText)
   {
      Options.ReadOptions.InputIndent = indentText;
   }

   void GlobalCommands::SetOutputIndent(const wstring& indentText)
   {
      Options.OutputLineIndent = indentText;
   }

   TreeCommandsPtr GlobalCommands::LoadTree(const filesystem::path& filename)
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

   NamedFilterPtr GlobalCommands::NameFilter(const wstring& filterName, const TreeCommandsPtr& tree)
   {
      tree->SetFilterName(filterName);
      return NameFilter(filterName, tree->GetFilter());
   }

   NamedFilterPtr GlobalCommands::NameFilter(const wstring& filterName, const TreeFilterPtr& filter)
   {
      return _knownFilters->Add(filterName, filter);
   }

   bool GlobalCommands::RemoveNamedFilter(const wstring& filterName)
   {
      return _knownFilters->Remove(filterName);
   }

   void GlobalCommands::SaveNamedFilters(const filesystem::path& filename)
   {
      if (_knownFilters->All().size() > 0)
         WriteNamedFilters(filename, *_knownFilters);
   }

   void GlobalCommands::LoadNamedFilters(const filesystem::path& filename)
   {
      auto filters = ReadNamedFilters(filename);
      _knownFilters->Merge(filters);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Options.

   void GlobalCommands::SaveOptions(const filesystem::path& filename)
   {
      wofstream stream(filename);
      SaveOptions(stream);
   }

   void GlobalCommands::SaveOptions(std::wostream& stream)
   {
      stream << L"V1: " << L"\n"
      << L"output-indent: "   << quoted(Options.OutputLineIndent) << L"\n"
      << L"input-filter: "    << quoted(Options.ReadOptions.InputFilter) << L"\n"
      << L"input-indent: "    << quoted(Options.ReadOptions.InputIndent) << L"\n"
      << L"tab-size: "        << Options.ReadOptions.TabSize << L"\n";
   }

   void GlobalCommands::LoadOptions(const filesystem::path& filename)
   {
      wifstream stream(filename);
      LoadOptions(stream);
   }

   void GlobalCommands::LoadOptions(std::wistream& stream)
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

   void GlobalCommands::RemoveTree(const TreeCommandsPtr& tree)
   {
      const auto pos = find(_trees.begin(), _trees.end(), tree);
      if (pos == _trees.end())
         return;

      _trees.erase(pos);
   }

   TreeCommandsPtr GlobalCommands::CreateTreeFromFiltered(const TreeCommandsPtr& tree)
   {
      if (!tree)
         return {};

      auto newCtx = make_shared<TreeCommands>(tree->GetFilteredTree(), tree->GetFilteredTreeFileName(), _knownFilters, _undoRedo);

      _trees.emplace_back(newCtx);

      return newCtx;
   }

   void GlobalCommands::ClearUndoStack()
   {
      _undoRedo->Clear();
   }

}
