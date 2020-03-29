#include "TreeCommands.h"
#include "GlobalCommands.h" // For options...
#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"
#include "SimpleTreeWriter.h"

#include <sstream>
#include <fstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

   TreeCommands::TreeCommands(TextTreePtr tree, wstring name, shared_ptr<NamedFilters> knownFilters, shared_ptr<UndoStack> undoRedo)
   : Tree(move(tree)), TreeFileName(move(name)), _knownFilters(move(knownFilters)), _undoRedo(move(undoRedo))
   {
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Filtered tree save.

   void TreeCommands::SaveFilteredTree(const filesystem::path& filename, const CommandsOptions& options)
   {
      if (Filtered)
      {
         WriteSimpleTextTree(filename, *Filtered, options.OutputLineIndent);
         FilteredFileName = filename;
         FilteredWasSaved = true;
      }
   }

   bool TreeCommands::IsFilteredTreeSaved() const
   {
      return FilteredWasSaved;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filter.

   void TreeCommands::SetFilter(const TreeFilterPtr& filter)
   {
      Filter = filter;
      CommitFilterToUndo();
   }

   const TreeFilterPtr& TreeCommands::GetFilter() const
   {
      return Filter;
   }

   const std::wstring& TreeCommands::GetFilterName() const
   {
      return FilterName;
   }

   void TreeCommands::SetFilterName(const std::wstring& name)
   {
      FilterName = name;
   }

   void TreeCommands::ApplyFilterToTree(bool async)
   {
      AbortAsyncFilter();

      if (Filter)
      {
         if (async)
         {
            _asyncFiltering = move(FilterTreeAsync(Tree, Filter));
         }
         else
         {
            Filtered = make_shared<TextTree>();
            FilterTree(*Tree, *Filtered, *Filter);
            FilteredWasSaved = false;
            ApplySearchInTree(async);
         }
      }
      else
      {
         Filtered = make_shared<TextTree>(*Tree);
         // Note: pure copy of input tree are considered to have been saved.
         FilteredWasSaved = true;
         ApplySearchInTree(async);
      }
   }

   void TreeCommands::AbortAsyncFilter()
   {
      if (_asyncFiltering.second)
         _asyncFiltering.second->Abort = true;
      _asyncFiltering = AsyncFilterTreeResult();

      AbortAsyncSearch();
   }

   bool TreeCommands::IsAsyncFilterReady()
   {
      if (_asyncFiltering.first.valid())
      {
         if (_asyncFiltering.first.wait_for(1us) != future_status::ready)
            return false;

         Filtered = make_shared<TextTree>(_asyncFiltering.first.get());
         FilteredWasSaved = false;
         _asyncFiltering = AsyncFilterTreeResult();

         ApplySearchInTree(true);
      }

      return IsAsyncSearchReady();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Text search.

   void TreeCommands::AbortAsyncSearch()
   {
      if (_asyncSearching.second)
         _asyncSearching.second->Abort = true;
      _asyncSearching = AsyncFilterTreeResult();
   }

   bool TreeCommands::IsAsyncSearchReady()
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

   void TreeCommands::SearchInTree(const std::wstring& text)
   {
      if (_searchedText == text)
         return;

      _searchedText = text;

      ApplySearchInTree(false);
   }

   void TreeCommands::SearchInTreeAsync(const std::wstring& text)
   {
      if (_searchedText == text)
         return;

      _searchedText = text;

      ApplySearchInTree(true);
   }

   void TreeCommands::ApplySearchInTree(bool async)
   {
      if (_searchedText.empty())
      {
         _searched = nullptr;
         return;
      }

      TextTreePtr applyTo = Filtered ? Filtered : Tree;

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

   TextTreePtr TreeCommands::GetOriginalTree() const
   {
      return Tree;
   }

   std::wstring TreeCommands::GetOriginalTreeFileName() const
   {
      return TreeFileName;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filtered tree.

   TextTreePtr TreeCommands::GetFilteredTree() const
   {
      return _searched ? _searched : Filtered;
   }

   std::wstring TreeCommands::GetFilteredTreeFileName() const
   {
      return FilteredFileName;
   }

   bool TreeCommands::CanCreateTreeFromFiltered() const
   {
      return Filtered != nullptr;
   }


   /////////////////////////////////////////////////////////////////////////
   //
   // Undo / redo.

   void TreeCommands::DeadedFilters(any& data)
   {
      data = ConvertFiltersToText(Filter);
   }

   void TreeCommands::AwakenFilters(const any& data)
   {
      // Note: do not call SetFilter as it would put it in undo/redo...
      Filter = ConvertTextToFilters(any_cast<wstring>(data), *_knownFilters);;
   }

   void TreeCommands::CommitFilterToUndo()
   {
      _undoRedo->Commit(
      {
         ConvertFiltersToText(Filter),
         [self = this](any& data) { self->DeadedFilters(data); },
         [self = this](const any& data) { self->AwakenFilters(data); }
      });
   }

}
