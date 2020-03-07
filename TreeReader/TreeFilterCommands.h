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
      bool UseV1 = false;
      bool IsInteractive = false;
      bool Debug = false;
      std::wstring OutputLineIndent = L"  ";

      ReadSimpleTextTreeOptions ReadOptions;

      bool operator!=(const CommandsOptions& other) const
      {
         return UseV1 != other.UseV1
            || IsInteractive != other.IsInteractive
            || Debug != other.Debug
            || OutputLineIndent != other.OutputLineIndent
            || ReadOptions != other.ReadOptions;
      }
   };

   struct CommandsContext
   {
      CommandsOptions Options;

      bool operator!=(const CommandsContext& other) const
      {
         return Options != other.Options
             || TreeFileName != other.TreeFileName
             || FilterText != other.FilterText;
      }

      std::wstring GetHelp() const; // TODO: move elsewhere

      void SetInputFilter(const std::wstring& filterRegex);
      void SetInputIndent(const std::wstring& indentText);
      void SetOutputIndent(const std::wstring& indentText);

      std::wstring LoadTree(const std::filesystem::path& filename);
      void CommandsContext::SaveFilteredTree(const std::filesystem::path& filename);
      bool IsFilteredTreeSaved() const { return FilteredWasSaved; }

      void AppendFilterText(const std::wstring& text); // TODO: move elsewhere
      void ClearFilterText(); // TODO: move elsewhere

      void SetFilter(const TreeFilterPtr& filter);
      const TreeFilterPtr& GetFilter() const { return Filter; }
      std::wstring CreateFilter();
      std::wstring CreateFilter(const std::wstring& filterText);

      NamedFilterPtr NameFilter(const std::wstring& filterName);
      NamedFilterPtr NameFilter(const std::wstring& filterName, const TreeFilterPtr& filter);
      bool RemoveNamedFilter(const std::wstring& filterName);
      std::vector<NamedFilterPtr> GetNamedFilters() const;

      std::wstring ListNamedFilters(); // TODO: move elsewhere

      void SaveNamedFilters(const std::filesystem::path& filename);
      void LoadNamedFilters(const std::filesystem::path& filename);

      void ApplyFilterToTree();

      std::shared_ptr<TextTree> GetCurrentTree() const;
      std::shared_ptr<TextTree> GetFilteredTree() const;
      void PushFilteredAsTree();
      void PopTree();

      void ClearUndoStack();
      void CommitToUndo();
      void Undo();
      void Redo();
      bool HasUndo() const;
      bool HasRedo() const;

      // TODO: move elsewhere
      std::wstring ParseCommands(const std::wstring& cmdText);
      std::wstring ParseCommands(const std::vector<std::wstring>& cmds);

   private:
      void DeadedFilters(std::any& data);
      void AwakenFilters(const std::any& data);
      void AwakenToEmptyFilters();

      std::wstring TreeFileName;
      std::vector<std::shared_ptr<TextTree>> Trees;

      std::wstring FilterText;
      TreeFilterPtr Filter;

      std::wstring FilteredFileName;
      std::shared_ptr<TextTree> Filtered;
      bool FilteredWasSaved = false;

      std::shared_ptr<NamedFilters> KnownFilters = std::make_shared<NamedFilters>();

      UndoStack UndoRedo;

   };

}
