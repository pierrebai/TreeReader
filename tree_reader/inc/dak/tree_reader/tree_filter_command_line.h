#pragma once

#include "dak/tree_reader/global_commands.h"

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Extra data used in teh command-line program.

   struct command_line_t : global_commands_t
   {
      // options.

      bool use_v1 = false;
      bool is_interactive = false;
      bool debug = false;

      tree_commands_ptr_t current_tree;

      // Help.

      std::wstring get_help() const;

      // filter creation.

      void append_filter_text(const std::wstring& text);
      void clear_filter_text();

      std::wstring create_filter();
      std::wstring create_filter(const std::wstring& filterText);

      // Named filters management.

      std::wstring list_filters();

      // Command parsing.

      std::wstring parse_commands(const std::wstring& cmdText);
      std::wstring parse_commands(const std::vector<std::wstring>& cmds);

      // Comparison with other command-line.

      bool operator!=(const command_line_t& other) const
      {
         return options        != other.options
             || use_v1         != other.use_v1
             || is_interactive != other.is_interactive
             || debug          != other.debug;
      }

   protected:
      std::wstring filter_text;
   };

}
