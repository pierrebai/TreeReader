#pragma once

#include "TextTree.h"
#include "TreeFilter.h"
#include "SimpleTreeReader.h"
#include "NamedFilters.h"

#include <memory>

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
      std::shared_ptr<TextTree> Filtered;

      NamedFilters NamedFilters;

      bool operator!=(const CommandsContext& other) const
      {
         return Options != other.Options
             || TreeFileName != other.TreeFileName
             || FilterText != other.FilterText;
      }
   };

   std::wstring ParseCommands(const std::wstring& cmdText, CommandsContext& ctx);
   std::wstring ParseCommands(const std::vector<std::wstring>& cmds, CommandsContext& ctx);
}
