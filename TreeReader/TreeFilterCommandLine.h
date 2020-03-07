#pragma once

#include "TreeFilterCommands.h"

namespace TreeReader
{
   struct CommandLine : CommandsContext
   {
      bool UseV1 = false;
      bool IsInteractive = false;
      bool Debug = false;

      bool operator!=(const CommandLine& other) const
      {
         return CommandsContext::operator!=(other)
             || UseV1         != other.UseV1
             || IsInteractive != other.IsInteractive
             || Debug         != other.Debug;
      }

      std::wstring GetHelp() const;

      void AppendFilterText(const std::wstring& text);
      void ClearFilterText();

      std::wstring CreateFilter();
      std::wstring CreateFilter(const std::wstring& filterText);

      std::wstring ListNamedFilters();

      std::wstring ParseCommands(const std::wstring& cmdText);
      std::wstring ParseCommands(const std::vector<std::wstring>& cmds);

   protected:
      std::wstring FilterText;
   };

}
