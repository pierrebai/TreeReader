#pragma once

#include "dak/tree_reader/text_tree.h"
#include "dak/tree_reader/tree_filter.h"
#include "dak/tree_reader/tree_filtering.h"
#include "dak/tree_reader/simple_tree_reader.h"
#include "dak/tree_reader/named_filters.h"
#include "dak/utility/undo_stack.h"

#include <memory>
#include <string>
#include <filesystem>

namespace dak::tree_reader
{
   struct tree_commands_t;
   using tree_commands_ptr_t = std::shared_ptr<tree_commands_t>;
   using undo_stack = utility::undo_stack_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // options for the command context.

   struct commands_options_t
   {
      std::wstring output_line_indent = L"  ";

      load_simple_text_tree_options_t read_options;

      bool operator!=(const commands_options_t& other) const
      {
         return output_line_indent != other.output_line_indent
             || read_options       != other.read_options;
      }
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // The global commands.
   //
   // These are commands that can be used in the application.
   //
   // Put here to isolate views from data and data from views.

   struct global_commands_t
   {
      // Global commands constructor.

      global_commands_t();

      // options.

      commands_options_t options;

      void save_options(const std::filesystem::path& filename);
      void save_options(std::wostream& stream);
      void load_options(const std::filesystem::path& filename);
      void load_options(std::wistream& stream);

      // Tree loading and saving options.

      void set_input_filter(const std::wstring& filterregex);
      void set_input_indentation(const std::wstring& indentText);
      void set_output_indentation(const std::wstring& indentText);

      // Tree loading, removing and creation of derived tree.

      tree_commands_ptr_t load_tree(const std::filesystem::path& filename);
      void remove_tree(const tree_commands_ptr_t& tree);
      tree_commands_ptr_t create_tree_from_filtered(const tree_commands_ptr_t& tree);

      // Named filters management.

      named_filter_ptr name_filter(const std::wstring& filterName, const tree_commands_ptr_t& tree);
      named_filter_ptr name_filter(const std::wstring& filterName, const tree_filter_ptr_t& filter);
      bool remove_Filter(const std::wstring& filterName);
      const named_filters_t& get_Filters() const { return *_known_filters; }

      void save_named_filters(const std::filesystem::path& filename);
      void load_named_filters(const std::filesystem::path& filename);

      // undo / redo.

      void clear_undo_stack();
      undo_stack& undo_redo() { return *_undo_redo; }

   protected:

      // The known trees being filtered.
      std::vector<tree_commands_ptr_t> _trees;

      // Known named filters.
      std::shared_ptr<named_filters_t> _known_filters = std::make_shared<named_filters_t>();

      // undo/redo stack.
      std::shared_ptr<undo_stack> _undo_redo = std::make_shared<undo_stack>();
   };

}
