#pragma once

#include "TextTree.h"
#include "TreeFilter.h"
#include "TreeFiltering.h"
#include "NamedFilters.h"
#include "UndoStack.h"

#include <memory>
#include <string>
#include <filesystem>

namespace TreeReader
{
   struct CommandsOptions;

   ////////////////////////////////////////////////////////////////////////////
   //
   // A single group of tree/filter/filtered result and
   // their associated commands
   //
   // The tree filter command context keeps a stack of these.

   struct TreeCommands
   {
      // Create a new tree command for the given tree.

      TreeCommands(TextTreePtr tree, std::wstring name, std::shared_ptr<NamedFilters> knownFilters, std::shared_ptr<UndoStack> undoRedo);

      // Current filter.

      void SetFilter(const TreeFilterPtr& filter);
      const TreeFilterPtr& GetFilter() const;
      const std::wstring& GetFilterName() const;
      void SetFilterName(const std::wstring& name);

      // Tree.

      TextTreePtr GetCurrentTree() const;
      std::wstring GetCurrentTreeFileName() const;

      // Filtered tree.

      TextTreePtr GetFilteredTree() const;
      std::wstring GetFilteredTreeFileName() const;
      bool CanCreateTreeFromFiltered() const;

      // Filtering.

      void ApplyFilterToTree(bool async = false);
      void ApplyFilterToTreeAsync() { ApplyFilterToTree(true); }
      void AbortAsyncFilter();
      bool IsAsyncFilterReady();

      // Searching.

      void SearchInTree(const std::wstring& text);
      void SearchInTreeAsync(const std::wstring& text);
      void AbortAsyncSearch();
      bool IsAsyncSearchReady();

      // Filtered tree save.

      void SaveFilteredTree(const std::filesystem::path& filename, CommandsOptions& options);
      bool IsFilteredTreeSaved() const;

   private:
      // Functions used for undo/redo.
      void DeadedFilters(std::any& data);
      void AwakenFilters(const std::any& data);
      void CommitFilterToUndo();

      void ApplySearchInTree(bool async);

      // Asynchronous filtering and searching.
      AsyncFilterTreeResult _asyncFiltering;
      AsyncFilterTreeResult _asyncSearching;

      // Search text. Applied on top of the filters.
      std::wstring _searchedText;
      TextTreePtr _searched;

      std::wstring TreeFileName;
      TextTreePtr Tree;

      std::wstring FilteredFileName;
      TextTreePtr Filtered;
      bool FilteredWasSaved = false;

      std::wstring FilterName;
      TreeFilterPtr Filter;

      // Known named filters.
      std::shared_ptr<NamedFilters> _knownFilters = std::make_shared<NamedFilters>();

      // Undo/redo stack.
      std::shared_ptr<UndoStack> _undoRedo;
   };

   using TreeCommandsPtr = std::shared_ptr<TreeCommands>;
}
