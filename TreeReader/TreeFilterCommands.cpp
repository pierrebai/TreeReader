#include "TreeFilterCommands.h"
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

   wstring CommandsContext::LoadTree(const filesystem::path& filename)
   {
      _treeFileName = filename;
      auto newTree = make_shared<TextTree>(ReadSimpleTextTree(filesystem::path(_treeFileName), Options.ReadOptions));
      if (newTree && newTree->Roots.size() > 0)
      {
         _trees.emplace_back(move(newTree));
         ApplySearchInTree(false);
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

   void CommandsContext::ApplyFilterToTree(bool async)
   {
      if (_trees.size() <= 0)
         return;

      AbortAsyncFilter();

      if (_filter)
      {
         if (async)
         {
            _asyncFiltering = move(FilterTreeAsync(_trees.back(), _filter));
         }
         else
         {
            _filtered = make_shared<TextTree>();
            FilterTree(*_trees.back(), *_filtered, _filter);
            _filteredWasSaved = false;
            ApplySearchInTree(async);
         }
      }
      else
      {
         _filtered = make_shared<TextTree>(*_trees.back());
         // Note: pure copy of input tree are considered to have been saved.
         _filteredWasSaved = true;
         ApplySearchInTree(async);
      }
   }

   void CommandsContext::AbortAsyncFilter()
   {
      if (_asyncFiltering.second)
         _asyncFiltering.second->Abort = true;
      _asyncFiltering = AsyncFilterTreeResult();

      AbortAsyncSearch();
   }

   bool CommandsContext::IsAsyncFilterReady()
   {
      if (_asyncFiltering.first.valid())
      {
         if (_asyncFiltering.first.wait_for(1us) != future_status::ready)
            return false;

         _filtered = make_shared<TextTree>(_asyncFiltering.first.get());
         _filteredWasSaved = false;
         _asyncFiltering = AsyncFilterTreeResult();

         ApplySearchInTree(true);
      }

      return IsAsyncSearchReady();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Text search.

   void CommandsContext::AbortAsyncSearch()
   {
      if (_asyncSearching.second)
         _asyncSearching.second->Abort = true;
      _asyncSearching = AsyncFilterTreeResult();
   }

   bool CommandsContext::IsAsyncSearchReady()
   {
      if (_asyncSearching.first.valid())
      {
         if (_asyncSearching.first.wait_for(1us) != future_status::ready)
            return false;

         _searched = make_shared<TextTree>(_asyncSearching.first.get());
         _asyncSearching = AsyncFilterTreeResult();
      }

      return true;
   }

   void CommandsContext::SearchInTree(const std::wstring& text)
   {
      if (_searchedText == text)
         return;

      _searchedText = text;

      ApplySearchInTree(false);
   }

   void CommandsContext::SearchInTreeAsync(const std::wstring& text)
   {
      if (_searchedText == text)
         return;

      _searchedText = text;

      ApplySearchInTree(true);
   }

   void CommandsContext::ApplySearchInTree(bool async)
   {
      if (_searchedText.empty())
      {
         _searched = nullptr;
         return;
      }

      shared_ptr<TextTree> applyTo = _filtered ? _filtered : _trees.size() > 0 ? _trees.back() : shared_ptr<TextTree>();

      if (!applyTo)
         return;

      auto filter = ConvertSimpleTextToFilters(_searchedText, *_knownFilters);

      AbortAsyncSearch();

      if (async)
      {
         _asyncSearching = move(FilterTreeAsync(applyTo, filter));
      }
      else
      {
         _searched = make_shared<TextTree>();
         FilterTree(*applyTo, *_searched, filter);
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Named filters management.

   NamedFilterPtr CommandsContext::NameFilter(const wstring& filterName)
   {
      return NameFilter(filterName, _filter);
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
   // Current text tree and filtered tree.

   shared_ptr<TextTree> CommandsContext::GetCurrentTree() const
   {
      if (_trees.size() <= 0)
         return {};

      return _trees.back();
   }

   shared_ptr<TextTree> CommandsContext::GetFilteredTree() const
   {
      return _searched ? _searched : _filtered;
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

      ApplySearchInTree(false);
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
