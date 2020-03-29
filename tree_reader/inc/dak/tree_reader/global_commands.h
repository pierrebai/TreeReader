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
   struct TreeCommands;
   using TreeCommandsPtr = std::shared_ptr<TreeCommands>;

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
   // The global commands.
   //
   // These are commands that can be used in the application.
   //
   // Put here to isolate views from data and data from views.

   struct GlobalCommands
   {
      // Global commands constructor.

      GlobalCommands();

      // Options.

      CommandsOptions Options;

      void SaveOptions(const std::filesystem::path& filename);
      void SaveOptions(std::wostream& stream);
      void LoadOptions(const std::filesystem::path& filename);
      void LoadOptions(std::wistream& stream);

      // Tree loading and saving options.

      void SetInputFilter(const std::wstring& filterRegex);
      void SetInputIndent(const std::wstring& indentText);
      void SetOutputIndent(const std::wstring& indentText);

      // Tree loading, removing and creation of derived tree.

      TreeCommandsPtr LoadTree(const std::filesystem::path& filename);
      void RemoveTree(const TreeCommandsPtr& tree);
      TreeCommandsPtr CreateTreeFromFiltered(const TreeCommandsPtr& tree);

      // Named filters management.

      NamedFilterPtr NameFilter(const std::wstring& filterName, const TreeCommandsPtr& tree);
      NamedFilterPtr NameFilter(const std::wstring& filterName, const TreeFilterPtr& filter);
      bool RemoveNamedFilter(const std::wstring& filterName);
      const NamedFilters& GetNamedFilters() const { return *_knownFilters; }

      void SaveNamedFilters(const std::filesystem::path& filename);
      void LoadNamedFilters(const std::filesystem::path& filename);

      // Undo / redo.

      void ClearUndoStack();
      UndoStack& UndoRedo() { return *_undoRedo; }

   protected:

      // The known trees being filtered.
      std::vector<TreeCommandsPtr> _trees;

      // Known named filters.
      std::shared_ptr<NamedFilters> _knownFilters = std::make_shared<NamedFilters>();

      // Undo/redo stack.
      std::shared_ptr<UndoStack> _undoRedo = std::make_shared<UndoStack>();
   };

}
