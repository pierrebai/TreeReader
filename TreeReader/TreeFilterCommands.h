#pragma once

#include "TextTree.h"
#include "TreeFilter.h"
#include "TreeFiltering.h"
#include "SimpleTreeReader.h"
#include "NamedFilters.h"
#include "UndoStack.h"

#include <memory>
#include <string>
#include <filesystem>

namespace TreeReader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Options for the command context.

   struct CommandsOptions
   {
      std::wstring OutputLineIndent = L"  ";

      ReadSimpleTextTreeOptions ReadOptions;

      bool operator!=(const CommandsOptions& other) const
      {
         return OutputLineIndent != other.OutputLineIndent
             || ReadOptions      != other.ReadOptions;
      }
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A single group of tree/filter/filtered result.
   //
   // The command context keeps a stack of these.

   struct TreeContext
   {
      std::wstring TreeFileName;
      TextTreePtr Tree;

      std::wstring FilteredFileName;
      TextTreePtr Filtered;
      bool FilteredWasSaved = false;

      std::wstring FilterName;
      TreeFilterPtr Filter;

      // Comparison with other tree context.

      bool operator!=(const TreeContext& other) const
      {
         return TreeFileName != other.TreeFileName
             || FilteredFileName != other.FilteredFileName
             || Filter != other.Filter;
      }
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // The command context.
   // These are commands that can be used in the application.
   //
   // Put here to isolate views from data and data from views.

   struct CommandsContext
   {
      // Context constructor.

      CommandsContext();

      // Options.

      CommandsOptions Options;

      void SaveOptions(const std::filesystem::path& filename);
      void SaveOptions(std::wostream& stream);
      void LoadOptions(const std::filesystem::path& filename);
      void LoadOptions(std::wistream& stream);

      // Tree loading and saving.

      void SetInputFilter(const std::wstring& filterRegex);
      void SetInputIndent(const std::wstring& indentText);
      void SetOutputIndent(const std::wstring& indentText);

      std::wstring LoadTree(const std::filesystem::path& filename);
      void SaveFilteredTree(const std::filesystem::path& filename);
      bool IsFilteredTreeSaved() const;

      // Current filter.

      void SetFilter(const TreeFilterPtr& filter);
      const TreeFilterPtr& GetFilter() const;
      const std::wstring& GetFilterName() const;

      // Filtering.

      void ApplyFilterToTree(bool async = false);
      void ApplyFilterToTreeAsync() { ApplyFilterToTree(true); }
      void AbortAsyncFilter();
      bool IsAsyncFilterReady();

      void SearchInTree(const std::wstring& text);
      void SearchInTreeAsync(const std::wstring& text);
      void AbortAsyncSearch();
      bool IsAsyncSearchReady();

      // Named filters management.

      NamedFilterPtr NameFilter(const std::wstring& filterName);
      NamedFilterPtr NameFilter(const std::wstring& filterName, const TreeFilterPtr& filter);
      bool RemoveNamedFilter(const std::wstring& filterName);
      const NamedFilters& GetNamedFilters() const { return *_knownFilters; }

      void SaveNamedFilters(const std::filesystem::path& filename);
      void LoadNamedFilters(const std::filesystem::path& filename);

      // Current text tree.

      const TreeContext& GetCurrentTreeContext() const;
      TextTreePtr GetCurrentTree() const;
      bool SetCurrentTree(const TextTreePtr& tree);
      std::wstring GetCurrentTreeFileName() const;

      bool CanRemoveCurrentTree() const;
      void RemoveCurrentTree();

      // Current filtered tree.

      TextTreePtr GetFilteredTree() const;
      bool CanCreateTreeFromFiltered() const;
      TextTreePtr CreateTreeFromFiltered();

      // Undo / redo.

      void ClearUndoStack();
      UndoStack& UndoRedo() { return _undoRedo; }

   protected:
      // Functions used for undo/redo.
      void DeadedFilters(std::any& data);
      void AwakenFilters(const std::any& data);
      void CommitFilterToUndo();

      TreeContext& GetCurrentTreeContext();

      void ApplySearchInTree(bool async);

      // A stack of tree being filtered.
      // Commands can make teh filtered result a new base tree.
      std::vector<TreeContext> _trees;
      size_t _currentTree = -1;

      // Asynchronous filtering and searching.
      AsyncFilterTreeResult _asyncFiltering;
      AsyncFilterTreeResult _asyncSearching;

      // Search text. Applied on top of the filters.
      std::wstring _searchedText;
      TextTreePtr _searched;

      // Known named filters.
      std::shared_ptr<NamedFilters> _knownFilters = std::make_shared<NamedFilters>();

      // Undo/redo stack.
      UndoStack _undoRedo;
   };

}
