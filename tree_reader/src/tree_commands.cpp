#include "dak/tree_reader/tree_commands.h"
#include "dak/tree_reader/global_commands.h" // For options...
#include "dak/tree_reader/tree_filter_maker.h"
#include "dak/utility/text.h"
#include "dak/tree_reader/simple_tree_writer.h"

#include <sstream>
#include <fstream>
#include <iomanip>

namespace dak::tree_reader
{
   using namespace std;

   tree_commands_t::tree_commands_t(text_tree_ptr_t tree, wstring name, shared_ptr<named_filters_t> knownFilters, shared_ptr<undo_stack> undoRedo)
   : _tree(move(tree)), _tree_filename(move(name)), _known_filters(move(knownFilters)), _undo_redo(move(undoRedo))
   {
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // _filtered tree save.

   void tree_commands_t::save_filtered_tree(const filesystem::path& filename, const commands_options_t& options)
   {
      if (_filtered)
      {
         save_simple_text_tree(filename, *_filtered, options.output_line_indent);
         _filtered_filename = filename;
         _filtered_was_saved = true;
      }
   }

   bool tree_commands_t::is_filtered_tree_saved() const
   {
      return _filtered_was_saved;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filter.

   void tree_commands_t::set_filter(const tree_filter_ptr_t& a_filter)
   {
      _filter = a_filter;
      commit_filter_to_undo();
   }

   const tree_filter_ptr_t& tree_commands_t::get_filter() const
   {
      return _filter;
   }

   const std::wstring& tree_commands_t::get_filter_name() const
   {
      return _filter_name;
   }

   void tree_commands_t::set_filter_name(const std::wstring& name)
   {
      _filter_name = name;
   }

   void tree_commands_t::apply_filter_to_tree(bool async)
   {
      abort_async_filter();

      if (_filter)
      {
         if (async)
         {
            _async_filtering = move(filter_tree_async(_tree, _filter));
         }
         else
         {
            _filtered = make_shared<text_tree_t>();
            filter_tree(*_tree, *_filtered, *_filter);
            _filtered_was_saved = false;
            apply_search_in_tree(async);
         }
      }
      else
      {
         _filtered = make_shared<text_tree_t>(*_tree);
         // note: pure copy of input tree are considered to have been saved.
         _filtered_was_saved = true;
         apply_search_in_tree(async);
      }
   }

   void tree_commands_t::abort_async_filter()
   {
      if (_async_filtering.second)
         _async_filtering.second->abort = true;
      _async_filtering = async_filter_tree_result_t();

      abort_async_search();
   }

   bool tree_commands_t::is_async_filter_ready()
   {
      if (_async_filtering.first.valid())
      {
         if (_async_filtering.first.wait_for(1us) != future_status::ready)
            return false;

         _filtered = make_shared<text_tree_t>(_async_filtering.first.get());
         _filtered_was_saved = false;
         _async_filtering = async_filter_tree_result_t();

         apply_search_in_tree(true);
      }

      return is_async_search_ready();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Text search.

   void tree_commands_t::abort_async_search()
   {
      if (_async_searching.second)
         _async_searching.second->abort = true;
      _async_searching = async_filter_tree_result_t();
   }

   bool tree_commands_t::is_async_search_ready()
   {
      if (_async_searching.first.valid())
      {
         if (_async_searching.first.wait_for(1us) != future_status::ready)
            return false;

         _searched = make_shared<text_tree_t>(_async_searching.first.get());
         _async_searching = async_filter_tree_result_t();
      }

      return true;
   }

   void tree_commands_t::search_in_tree(const std::wstring& text)
   {
      if (_searched_text == text)
         return;

      _searched_text = text;

      apply_search_in_tree(false);
   }

   void tree_commands_t::search_in_tree_async(const std::wstring& text)
   {
      if (_searched_text == text)
         return;

      _searched_text = text;

      apply_search_in_tree(true);
   }

   void tree_commands_t::apply_search_in_tree(bool async)
   {
      if (_searched_text.empty())
      {
         _searched = nullptr;
         return;
      }

      text_tree_ptr_t applyTo = _filtered ? _filtered : _tree;

      if (!applyTo)
         return;

      auto _filter = convert_simple_text_to_filter(_searched_text, *_known_filters);
      if (!_filter)
         return;

      abort_async_search();

      if (async)
      {
         _async_searching = move(filter_tree_async(applyTo, _filter));
      }
      else
      {
         _searched = make_shared<text_tree_t>();
         filter_tree(*applyTo, *_searched, *_filter);
      }
   }

   text_tree_ptr_t tree_commands_t::get_original_tree() const
   {
      return _tree;
   }

   std::wstring tree_commands_t::get_original_tree_filename() const
   {
      return _tree_filename;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current filtered tree.

   text_tree_ptr_t tree_commands_t::get_filtered_tree() const
   {
      return _searched ? _searched : _filtered;
   }

   std::wstring tree_commands_t::get_filtered_tree_filename() const
   {
      return _filtered_filename;
   }

   bool tree_commands_t::can_create_tree_from_filtered() const
   {
      return _filtered != nullptr;
   }


   /////////////////////////////////////////////////////////////////////////
   //
   // undo / redo.

   void tree_commands_t::deaden_filters(std::any& data)
   {
      data = convert_filter_to_text(_filter);
   }

   void tree_commands_t::awaken_filters(const std::any& data)
   {
      // note: do not call set_filter as it would put it in undo/redo...
      _filter = convert_text_to_filter(any_cast<wstring>(data), *_known_filters);;
   }

   void tree_commands_t::commit_filter_to_undo()
   {
      _undo_redo->simple_commit(
      {
         convert_filter_to_text(_filter),
         [self = this](std::any& data) { self->deaden_filters(data); },
         [self = this](const std::any& data) { self->awaken_filters(data); }
      });
   }

}
