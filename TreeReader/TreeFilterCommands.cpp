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
      // Ensure there is always at least one tree context.
      _trees.emplace_back();
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
         auto& ctx = _trees.back();

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
      auto& ctx = _trees.back();

      if (ctx.Filtered)
      {
         WriteSimpleTextTree(filename, *ctx.Filtered, Options.OutputLineIndent);
         ctx.FilteredFileName = filename;
         ctx.FilteredWasSaved = true;
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filter.

   void CommandsContext::SetFilter(const TreeFilterPtr& filter)
   {
      auto& ctx = _trees.back();

      ctx.Filter = filter;
      CommitFilterToUndo();
   }

   void CommandsContext::ApplyFilterToTree(bool async)
   {
      auto& ctx = _trees.back();

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
      auto& ctx = _trees.back();

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
      auto& ctx = _trees.back();

      if (_searchedText.empty())
      {
         _searched = nullptr;
         return;
      }

      shared_ptr<TextTree> applyTo = ctx.Filtered ? ctx.Filtered : _trees.size() > 0 ? ctx.Tree : shared_ptr<TextTree>();

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
      auto& ctx = _trees.back();
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
   // Current text tree and filtered tree.

   shared_ptr<TextTree> CommandsContext::GetCurrentTree() const
   {
      auto& ctx = _trees.back();

      if (_trees.size() <= 0)
         return {};

      return ctx.Tree;
   }

   shared_ptr<TextTree> CommandsContext::GetFilteredTree() const
   {
      auto& ctx = _trees.back();

      return _searched ? _searched : ctx.Filtered;
   }

   bool CommandsContext::CanPushTree() const
   {
      auto& ctx = _trees.back();

      return ctx.Filtered != nullptr;
   }

   bool CommandsContext::CanPopTree() const
   {
      return (_trees.size() > 1);
   }

   void CommandsContext::PushFilteredAsTree()
   {
      if (!CanPushTree())
         return;

      auto& ctx = _trees.back();

      TreeContext newCtx;
      newCtx.Tree = ctx.Filtered;
      newCtx.TreeFileName = ctx.FilteredFileName;

      _trees.emplace_back(move(newCtx));
   }
   
   void CommandsContext::PopTree()
   {
      if (!CanPopTree())
         return;

      _trees.pop_back();

      ApplySearchInTree(false);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Undo / redo.

   void CommandsContext::DeadedFilters(any& data)
   {
      auto& ctx = _trees.back();

      data = ConvertFiltersToText(ctx.Filter);
   }

   void CommandsContext::AwakenFilters(const any& data)
   {
      auto& ctx = _trees.back();

      // Note: do not call SetFilter as it would put it in undo/redo...
      ctx.Filter = ConvertTextToFilters(any_cast<wstring>(data), *_knownFilters);;
   }

   void CommandsContext::ClearUndoStack()
   {
      _undoRedo.Clear();
   }

   void CommandsContext::CommitFilterToUndo()
   {
      auto& ctx = _trees.back();

      _undoRedo.Commit(
      {
         ConvertFiltersToText(ctx.Filter),
         [self = this](any& data) { self->DeadedFilters(data); },
         [self = this](const any& data) { self->AwakenFilters(data); }
      });
   }
    
}
