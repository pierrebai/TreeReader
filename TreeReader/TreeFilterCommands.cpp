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

   wstring CommandsContext::LoadTree(const filesystem::path& filename)
   {
      auto newTree = make_shared<TextTree>(ReadSimpleTextTree(filename, Options.ReadOptions));
      if (newTree && newTree->Roots.size() > 0)
      {
         _trees.emplace_back();
         _currentTree = _trees.size() - 1;

         auto& ctx = GetCurrentTreeContext();

         ctx.TreeFileName = filename;
         ctx.Tree = newTree;

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
      auto& ctx = GetCurrentTreeContext();

      if (ctx.Filtered)
      {
         WriteSimpleTextTree(filename, *ctx.Filtered, Options.OutputLineIndent);
         ctx.FilteredFileName = filename;
         ctx.FilteredWasSaved = true;
      }
   }

   bool CommandsContext::IsFilteredTreeSaved() const
   {
      return GetCurrentTreeContext().FilteredWasSaved;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filter.

   void CommandsContext::SetFilter(const TreeFilterPtr& filter)
   {
      auto& ctx = GetCurrentTreeContext();

      ctx.Filter = filter;
      CommitFilterToUndo();
   }

   const TreeFilterPtr& CommandsContext::GetFilter() const
   {
      return GetCurrentTreeContext().Filter;
   }

   const std::wstring& CommandsContext::GetFilterName() const
   {
      return GetCurrentTreeContext().FilterName;
   }

   void CommandsContext::ApplyFilterToTree(bool async)
   {
      auto& ctx = GetCurrentTreeContext();

      AbortAsyncFilter();

      if (ctx.Filter)
      {
         if (async)
         {
            _asyncFiltering = move(FilterTreeAsync(ctx.Tree, ctx.Filter));
         }
         else
         {
            ctx.Filtered = make_shared<TextTree>();
            FilterTree(*ctx.Tree, *ctx.Filtered, *ctx.Filter);
            ctx.FilteredWasSaved = false;
            ApplySearchInTree(async);
         }
      }
      else
      {
         ctx.Filtered = make_shared<TextTree>(*ctx.Tree);
         // Note: pure copy of input tree are considered to have been saved.
         ctx.FilteredWasSaved = true;
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
      auto& ctx = GetCurrentTreeContext();

      if (_asyncFiltering.first.valid())
      {
         if (_asyncFiltering.first.wait_for(1us) != future_status::ready)
            return false;

         ctx.Filtered = make_shared<TextTree>(_asyncFiltering.first.get());
         ctx.FilteredWasSaved = false;
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
      auto& ctx = GetCurrentTreeContext();

      if (_searchedText.empty())
      {
         _searched = nullptr;
         return;
      }

      TextTreePtr applyTo = ctx.Filtered ? ctx.Filtered : ctx.Tree;

      if (!applyTo)
         return;

      auto filter = ConvertSimpleTextToFilters(_searchedText, *_knownFilters);
      if (!filter)
         return;

      AbortAsyncSearch();

      if (async)
      {
         _asyncSearching = move(FilterTreeAsync(applyTo, filter));
      }
      else
      {
         _searched = make_shared<TextTree>();
         FilterTree(*applyTo, *_searched, *filter);
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Named filters management.

   NamedFilterPtr CommandsContext::NameFilter(const wstring& filterName)
   {
      auto& ctx = GetCurrentTreeContext();
      ctx.FilterName = filterName;

      return NameFilter(filterName, ctx.Filter);
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

   const TreeContext& CommandsContext::GetCurrentTreeContext() const
   {
      return const_cast<CommandsContext*>(this)->GetCurrentTreeContext();
   }

   TreeContext& CommandsContext::GetCurrentTreeContext()
   {
      if (_currentTree < 0 || _currentTree >= _trees.size())
      {
         static TreeContext empty;
         return empty;
      }

      return _trees[_currentTree];
   }

   TextTreePtr CommandsContext::GetCurrentTree() const
   {
      auto& ctx = GetCurrentTreeContext();

      return ctx.Tree;
   }

   bool CommandsContext::SetCurrentTree(const TextTreePtr& tree)
   {
      for (size_t i = 0; i < _trees.size(); ++i)
      {
         if (_trees[i].Tree == tree)
         {
            if (_currentTree != i)
            {
               _currentTree = i;
               return true;
            }
            break;
         }
      }

      return false;
   }

   bool CommandsContext::CanRemoveCurrentTree() const
   {
      return _trees.size() > 0;
   }

   void CommandsContext::RemoveCurrentTree()
   {
      if (!CanRemoveCurrentTree())
         return;

      _trees.erase(_trees.begin() + _currentTree);
      _currentTree -= 1;

      ApplySearchInTree(false);
   }

   std::wstring CommandsContext::GetCurrentTreeFileName() const
   {
      auto& ctx = GetCurrentTreeContext();

      return ctx.TreeFileName;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filtered tree.

   TextTreePtr CommandsContext::GetFilteredTree() const
   {
      auto& ctx = GetCurrentTreeContext();

      return _searched ? _searched : ctx.Filtered;
   }

   bool CommandsContext::CanCreateTreeFromFiltered() const
   {
      auto& ctx = GetCurrentTreeContext();

      return ctx.Filtered != nullptr;
   }

   TextTreePtr CommandsContext::CreateTreeFromFiltered()
   {
      if (!CanCreateTreeFromFiltered())
         return {};

      auto& ctx = GetCurrentTreeContext();

      TreeContext newCtx;
      newCtx.Tree = ctx.Filtered;
      newCtx.TreeFileName = ctx.FilteredFileName;

      _trees.emplace_back(move(newCtx));
      _currentTree = _trees.size() - 1;

      return GetCurrentTree();
   }
   
   /////////////////////////////////////////////////////////////////////////
   //
   // Undo / redo.

   void CommandsContext::DeadedFilters(any& data)
   {
      auto& ctx = GetCurrentTreeContext();

      data = ConvertFiltersToText(ctx.Filter);
   }

   void CommandsContext::AwakenFilters(const any& data)
   {
      auto& ctx = GetCurrentTreeContext();

      // Note: do not call SetFilter as it would put it in undo/redo...
      ctx.Filter = ConvertTextToFilters(any_cast<wstring>(data), *_knownFilters);;
   }

   void CommandsContext::ClearUndoStack()
   {
      _undoRedo.Clear();
   }

   void CommandsContext::CommitFilterToUndo()
   {
      auto& ctx = GetCurrentTreeContext();

      _undoRedo.Commit(
      {
         ConvertFiltersToText(ctx.Filter),
         [self = this](any& data) { self->DeadedFilters(data); },
         [self = this](const any& data) { self->AwakenFilters(data); }
      });
   }
    
}
