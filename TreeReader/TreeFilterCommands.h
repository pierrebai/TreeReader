#pragma once

#include "TextTree.h"
#include "TreeFilter.h"
#include "SimpleTreeReader.h"
#include "NamedFilters.h"

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

      std::wstring TreeFileName;
      std::vector<std::shared_ptr<TextTree>> Trees;

      std::wstring FilterText;
      TreeFilterPtr Filter;

      std::wstring FilteredFileName;
      std::shared_ptr<TextTree> Filtered;
      bool FilteredWasSaved = false;

      std::shared_ptr<NamedFilters> KnownFilters = std::make_shared<NamedFilters>();

      bool operator!=(const CommandsContext& other) const
      {
         return Options != other.Options
             || TreeFileName != other.TreeFileName
             || FilterText != other.FilterText;
      }

      std::wstring GetHelp() const;

      void SetInputFilter(const std::wstring& filterRegex);
      void SetInputIndent(const std::wstring& indentText);
      void SetOutputIndent(const std::wstring& indentText);

      std::wstring LoadTree(const std::filesystem::path& filename);
      void CommandsContext::SaveFilteredTree(const std::filesystem::path& filename);

      void AppendFilterText(const std::wstring& text);
      void ClearFilterText();

      std::wstring CreateFilter();
      std::wstring CreateFilter(const std::wstring& filterText);

      NamedFilterPtr NameFilter(const std::wstring& filterName);
      NamedFilterPtr NameFilter(const std::wstring& filterName, const TreeFilterPtr& filter);

      std::wstring ListNamedFilters();

      void SaveNamedFilters(const std::filesystem::path& filename);
      void LoadNamedFilters(const std::filesystem::path& filename);

      void ApplyFilterToTree();

      void PushFilteredAsTree();
      void PopTree();
   };

   std::wstring ParseCommands(const std::wstring& cmdText, CommandsContext& ctx);
   std::wstring ParseCommands(const std::vector<std::wstring>& cmds, CommandsContext& ctx);
}
