#pragma once

#include "GlobalCommands.h"

namespace TreeReader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Extra data used in teh command-line program.

   struct CommandLine : GlobalCommands
   {
      // Options.

      bool UseV1 = false;
      bool IsInteractive = false;
      bool Debug = false;

      TreeCommandsPtr CurrentTree;

      // Help.

      std::wstring GetHelp() const;

      // Filter creation.

      void AppendFilterText(const std::wstring& text);
      void ClearFilterText();

      std::wstring CreateFilter();
      std::wstring CreateFilter(const std::wstring& filterText);

      // Named filters management.

      std::wstring ListNamedFilters();

      // Command parsing.

      std::wstring ParseCommands(const std::wstring& cmdText);
      std::wstring ParseCommands(const std::vector<std::wstring>& cmds);

      // Comparison with other command-line.

      bool operator!=(const CommandLine& other) const
      {
         return Options       != other.Options
             || UseV1         != other.UseV1
             || IsInteractive != other.IsInteractive
             || Debug         != other.Debug;
      }

   protected:
      std::wstring FilterText;
   };

}
