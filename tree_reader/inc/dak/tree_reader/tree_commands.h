#pragma once

#include "dak/tree_reader/text_tree.h"
#include "dak/tree_reader/tree_filter.h"
#include "dak/tree_reader/tree_filtering.h"
#include "dak/tree_reader/named_filters.h"
#include "dak/utility/undo_stack.h"

#include <memory>
#include <string>
#include <filesystem>

namespace dak::tree_reader
{
   struct commands_options;
   using undo_stack = utility::undo_stack;

   ////////////////////////////////////////////////////////////////////////////
   //
   // A single group of tree/filter/filtered result and
   // their associated commands
   //
   // The tree filter command context keeps a stack of these.

   struct tree_commands
   {
      // Create a new tree command for the given tree.

      tree_commands(text_tree_ptr tree, std::wstring name, std::shared_ptr<named_filters> knownFilters, std::shared_ptr<undo_stack> undoRedo);

      // Current filter.

      void set_filter(const tree_filter_ptr& filter);
      const tree_filter_ptr& get_filter() const;
      const std::wstring& get_filter_name() const;
      void set_filter_name(const std::wstring& name);

      // Tree.

      text_tree_ptr get_original_tree() const;
      std::wstring get_original_tree_filename() const;

      // _filtered tree.

      text_tree_ptr get_filtered_tree() const;
      std::wstring get_filtered_tree_filename() const;
      bool can_create_tree_from_filtered() const;

      // Filtering.

      void apply_filter_to_tree(bool async = false);
      void apply_filter_to_tree_async() { apply_filter_to_tree(true); }
      void abort_async_filter();
      bool is_async_filter_ready();

      // Searching.

      void search_in_tree(const std::wstring& text);
      void search_in_tree_async(const std::wstring& text);
      void abort_async_search();
      bool is_async_search_ready();
      void apply_search_in_tree(bool async);

      // _filtered tree save.

      static constexpr char tree_file_types[] = "Text Tree files (*.txt *.log);;Text files (*.txt);;Log files (*.log)";
      void save_filtered_tree(const std::filesystem::path& filename, const commands_options& options);
      bool is_filtered_tree_saved() const;

   private:
      // functions used for undo/redo.
      void deaden_filters(std::any& data);
      void awaken_filters(const std::any& data);
      void commit_filter_to_undo();

      // Asynchronous filtering and searching.
      async_filter_tree_result _async_filtering;
      async_filter_tree_result _async_searching;

      // Search text. Applied on top of the filters.
      std::wstring _searched_text;
      text_tree_ptr _searched;

      std::wstring _tree_filename;
      text_tree_ptr Tree;

      std::wstring _filtered_filename;
      text_tree_ptr _filtered;
      bool _filtered_was_saved = false;

      std::wstring _filter_name;
      tree_filter_ptr _filter;

      // Known named filters.
      std::shared_ptr<named_filters> _known_filters = std::make_shared<named_filters>();

      // undo/redo stack.
      std::shared_ptr<undo_stack> _undo_redo;
   };

   using tree_commands_ptr = std::shared_ptr<tree_commands>;
}
