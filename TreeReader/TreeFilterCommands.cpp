#include "TreeFilterCommands.h"
#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"
#include "SimpleTreeWriter.h"

#include <sstream>

namespace TreeReader
{
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree loading and saving.

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
      _treeFileName = filename;
      auto newTree = make_shared<TextTree>(ReadSimpleTextTree(filesystem::path(_treeFileName), Options.ReadOptions));
      if (newTree && newTree->Roots.size() > 0)
      {
         _trees.emplace_back(move(newTree));
         return {};
      }
      else
      {
         return L"Tree file was invalid or empty.\n";
      }
   }

   void CommandsContext::SaveFilteredTree(const filesystem::path& filename)
   {
      _filteredFileName = filename;
      if (_filtered)
      {
         WriteSimpleTextTree(filesystem::path(_filteredFileName), *_filtered, Options.OutputLineIndent);
         _filteredWasSaved = true;
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filter.

   void CommandsContext::SetFilter(const TreeFilterPtr& filter)
   {
      _filter = filter;
      CommitFilterToUndo();
   }

   void CommandsContext::ApplyFilterToTree()
   {
      if (_trees.size() <= 0)
         return;

      if (_filter)
      {
         _filtered = make_shared<TextTree>();
         FilterTree(*_trees.back(), *_filtered, _filter);
         _filteredWasSaved = false;
      }
      else
      {
         _filtered = make_shared<TextTree>(*_trees.back());
         // Note: pure copy of input tree are considered to have been saved.
         _filteredWasSaved = true;
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Named filters management.

   NamedFilterPtr CommandsContext::NameFilter(const std::wstring& filterName)
   {
      return NameFilter(filterName, _filter);
   }

   NamedFilterPtr CommandsContext::NameFilter(const std::wstring& filterName, const TreeFilterPtr& filter)
   {
      return _knownFilters->Add(filterName, filter);
   }

   bool CommandsContext::RemoveNamedFilter(const std::wstring& filterName)
   {
      return _knownFilters->Remove(filterName);
   }

   vector<NamedFilterPtr> CommandsContext::GetNamedFilters() const
   {
      vector<NamedFilterPtr> filters;

      for (const auto& [name, filter] : _knownFilters->All())
         filters.push_back(filter);

      return filters;
   }

   void CommandsContext::SaveNamedFilters(const std::filesystem::path& filename)
   {
      if (_knownFilters->All().size() > 0)
         WriteNamedFilters(filename, *_knownFilters);
   }

   void CommandsContext::LoadNamedFilters(const std::filesystem::path& filename)
   {
      auto filters = ReadNamedFilters(filename);
      _knownFilters->Merge(filters);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current text tree and filtered tree.

   std::shared_ptr<TextTree> CommandsContext::GetCurrentTree() const
   {
      if (_trees.size() <= 0)
         return {};

      return _trees.back();
   }

   std::shared_ptr<TextTree> CommandsContext::GetFilteredTree() const
   {
      return _filtered;
   }

   void CommandsContext::PushFilteredAsTree()
   {
      if (_filtered)
      {
         _trees.emplace_back(move(_filtered));
      }
   }
   
   void CommandsContext::PopTree()
   {
      if (_trees.size() > 0)
         _trees.pop_back();
   }
   
   /////////////////////////////////////////////////////////////////////////
   //
   // Undo / redo.

   void CommandsContext::DeadedFilters(any& data)
   {
      data = ConvertFiltersToText(_filter);
   }

   void CommandsContext::AwakenFilters(const any& data)
   {
      // Note: do not call SetFilter as it would put it in undo/redo...
      _filter = ConvertTextToFilters(any_cast<wstring>(data), *_knownFilters);;
   }

   void CommandsContext::ClearUndoStack()
   {
      _undoRedo.Clear();
   }

   void CommandsContext::CommitFilterToUndo()
   {
      _undoRedo.Commit(
         {
            ConvertFiltersToText(_filter),
            [self = this](any& data) { self->DeadedFilters(data); },
            [self = this](const any& data) { self->AwakenFilters(data); }
         });
   }
    
}
