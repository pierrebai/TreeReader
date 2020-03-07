#pragma once

#include "TextTree.h"
#include "TreeFilter.h"
#include "SimpleTreeReader.h"
#include "NamedFilters.h"
#include "UndoStack.h"

#include <memory>
#include <string>
#include <filesystem>

namespace TreeReader
{
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

   struct CommandsContext
   {
      // Options.

      CommandsOptions Options;

      // Tree loading and saving.

      void SetInputFilter(const std::wstring& filterRegex);
      void SetInputIndent(const std::wstring& indentText);
      void SetOutputIndent(const std::wstring& indentText);

      std::wstring LoadTree(const std::filesystem::path& filename);
      void CommandsContext::SaveFilteredTree(const std::filesystem::path& filename);
      bool IsFilteredTreeSaved() const { return FilteredWasSaved; }

      // Current filter.

      void SetFilter(const TreeFilterPtr& filter);
      const TreeFilterPtr& GetFilter() const { return Filter; }
      void ApplyFilterToTree();

      // Named filters management.

      NamedFilterPtr NameFilter(const std::wstring& filterName);
      NamedFilterPtr NameFilter(const std::wstring& filterName, const TreeFilterPtr& filter);
      bool RemoveNamedFilter(const std::wstring& filterName);
      std::vector<NamedFilterPtr> GetNamedFilters() const;

      void SaveNamedFilters(const std::filesystem::path& filename);
      void LoadNamedFilters(const std::filesystem::path& filename);

      // Current text tree and filtered tree.

      std::shared_ptr<TextTree> GetCurrentTree() const;
      std::shared_ptr<TextTree> GetFilteredTree() const;
      void PushFilteredAsTree();
      void PopTree();

      // Undo / redo.

      void ClearUndoStack();
      void Undo();
      void Redo();
      bool HasUndo() const;
      bool HasRedo() const;

   protected:
      void DeadedFilters(std::any& data);
      void AwakenFilters(const std::any& data);
      void AwakenToEmptyFilters();
      void CommitFilterToUndo();

      std::wstring TreeFileName;
      std::vector<std::shared_ptr<TextTree>> Trees;

      TreeFilterPtr Filter;

      std::wstring FilteredFileName;
      std::shared_ptr<TextTree> Filtered;
      bool FilteredWasSaved = false;

      std::shared_ptr<NamedFilters> KnownFilters = std::make_shared<NamedFilters>();

      UndoStack UndoRedo;

   };

}
