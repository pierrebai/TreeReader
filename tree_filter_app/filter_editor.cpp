#include "filter_editor.h"
#include "tree_filter_list_widget.h"

#include "dak/QtAdditions/QtUtilities.h"
#include "dak/QtAdditions/QWidgetScrollListWidget.h"

#include "dak/tree_reader/tree_filter.h"
#include "dak/tree_reader/tree_filter_helpers.h"
#include "dak/tree_reader/named_filters.h"
#include "dak/tree_reader/tree_filter_maker.h"

#include "dak/utility/undo_stack.h"
#include "dak/utility/exceptions.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qscrollarea.h>

#include <algorithm>
#include <typeindex>

namespace dak::tree_reader::app
{
   using namespace dak::tree_reader;
   using namespace QtAdditions;
   using namespace std;

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   class filters_editor_ui
   {
   public:
      filters_editor_ui(const named_filters_t& known, undo_stack& undoRedo, filter_editor_t& parent)
      : _known_filters(known), _undo_redo(undoRedo), _editor(parent)
      {
         build_ui(parent);
         connect_ui();
      }

      tree_filter_ptr_t get_edited()
      {
         update_editor_from_ui();
         return _edited;
      }

      wstring get_edited_name() const
      {
         return _filter_name;
      }

      void set_edited(const tree_filter_ptr_t& ed, const wstring& name, bool forced)
      {
         if (ed == _edited && !forced)
            return;

         _edited = ed;
         _filter_name = name;

         fill_ui();

         commit_filter_changes_to_undo();
      }

   private:

      /////////////////////////////////////////////////////////////////////////
      //
      // UI setup.

      void build_ui(filter_editor_t& parent)
      {
         QVBoxLayout* layout = new QVBoxLayout(&parent);
         layout->setContentsMargins(0, 0, 0, 0);

         auto del_func = [self = this](tree_filter_list_item_t* panel)
         {
            // When a filter gets deleted, update the edited filter if it is no
            // longer the root.
            delete panel;
            auto filters = self->_filter_list->get_tree_filters();
            if (filters.size() > 0)
               self->_edited = filters.back();
            else
               self->_edited = nullptr;

            self->commit_filter_changes_to_undo();
         };

         auto list_changed_callback = [self = this](QWidgetListWidget* list)
         {
            self->commit_filter_changes_to_undo();
         };

         _filter_list = new tree_filter_list_widget_t(del_func, {}, list_changed_callback);

         _filter_list->setAcceptDrops(true);
         _filter_list->setEnabled(true);

         _filter_scroll = new QWidgetScrollListWidget(_filter_list);
         layout->addWidget(_filter_scroll);

         // note: allow undoing back to an empty filter list. To enable this, there must be an empty initial commit.
         _undo_redo.simple_commit({ 0, nullptr, [self = this](const std::any&) { self->awaken_to_empty_filters(); } });
      }

      void connect_ui()
      {
      }

      static bool is_under(const tree_filter_ptr_t& filter, const tree_filter_ptr_t& child)
      {
         if (filter == child)
            return true;

         if (auto combine = dynamic_pointer_cast<combine_tree_filter_t>(filter))
         {
            for (const auto& c : combine->filters)
               if (is_under(c, child))
                  return true;
         }
         else if (auto delegate = dynamic_pointer_cast<delegate_tree_filter_t>(filter))
         {
            return is_under(delegate->sub_filter, child);
         }

         return false;
      }

      void fill_ui()
      {
         DisableFeedback df(_filter_list, _disable_feedback);

         deque<pair<shared_ptr<combine_tree_filter_t>, tree_filter_list_widget_t*>> combineFilters;

         _filter_list->clear();
         dak::tree_reader::visit_filters(_edited, true, [self = this, &combineFilters](const tree_filter_ptr_t& filter) -> bool
         {
            QWidgetListItem* widget = nullptr;
            for (auto& [combineFilter, combineWidget] : combineFilters)
            {
               if (is_under(combineFilter, filter))
               {
                  widget = combineWidget->add_tree_filter(filter);
                  break;
               }
            }


            if (!widget)
               widget = self->_filter_list->add_tree_filter(filter);

            if (tree_filter_list_item_t* filterWidget = dynamic_cast<tree_filter_list_item_t*>(widget))
            {
               if (filterWidget->sub_list)
               {
                  if (auto combine = dynamic_pointer_cast<combine_tree_filter_t>(filter))
                  {
                     combineFilters.emplace_front(combine, filterWidget->sub_list);
                  }
               }
            }

            return true;
         });
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // Transfer from UI to data.

      static tree_filter_ptr_t disconnect_filter(tree_filter_ptr_t filter)
      {
         if (auto delegate = dynamic_pointer_cast<delegate_tree_filter_t>(filter))
         {
            delegate->sub_filter = nullptr;
         }
         else if (auto combine = dynamic_pointer_cast<combine_tree_filter_t>(filter))
         {
            combine->filters.clear();
         }
         return filter;
      }

      static void fill_filters_from_ui(const std::vector<QWidgetListItem*>& widgets, tree_filter_ptr_t& root, vector<tree_filter_ptr_t>& previous)
      {
         for (auto& widget : widgets)
         {
            auto tfw = dynamic_cast<tree_filter_list_item_t*>(widget);
            if (!tfw)
               continue;
            if (!tfw->filter)
               continue;

            auto filter = disconnect_filter(tfw->filter->clone());

            if (!root)
            {
               root = filter;
            }
            else
            {
               bool placed = false;
               while (previous.size() > 0)
               {
                  if (auto delegate = dynamic_pointer_cast<delegate_tree_filter_t>(previous.back()))
                  {
                     if (!delegate->sub_filter)
                     {
                        delegate->sub_filter = filter;
                        placed = true;
                        break;
                     }
                  }
                  else if (auto combine = dynamic_pointer_cast<combine_tree_filter_t>(previous.back()))
                  {
                     combine->filters.push_back(filter);
                     placed = true;
                     break;
                  }

                  previous.pop_back();
               }

               if (!placed)
               {
                  auto new_root = and(root, filter);
                  root = new_root;
                  previous.push_back(root);
               }
            }

            if (tfw->sub_list)
            {
               vector<tree_filter_ptr_t> sub_previous;
               sub_previous.push_back(filter);
               fill_filters_from_ui(tfw->sub_list->getItems(), filter, sub_previous);

               // note: when we have an explicit sub-list, we don't use it as
               //       the previous implicit combine to put other filters not
               //       under it in.
               //
               //       So we don't put it in the previous stack in this
               //       level.
            }
            else
            {
               previous.push_back(filter);
            }
         }
      }

      void update_editor_from_ui()
      {
         vector<tree_filter_ptr_t> previous;
         tree_filter_ptr_t root;
         fill_filters_from_ui(_filter_list->getItems(), root, previous);
         _edited = root;
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // undo/redo.

      void commit_filter_changes_to_undo()
      {
         if (_disable_feedback > 0)
            return;

         update_editor_from_ui();
         _undo_redo.simple_commit(
            {
               convert_filter_to_text(_edited),
               [self = this](std::any& data) { self->deaden_filters(data); },
               [self = this](const std::any& data) { self->awaken_filters(data); }
            });
      }

      void deaden_filters(std::any& data)
      {
         update_editor_from_ui();
         data = convert_filter_to_text(_edited);
      }

      void awaken_filters(const std::any& data)
      {
         _edited = convert_text_to_filter(any_cast<wstring>(data), _known_filters);
         fill_ui();
      }

      void awaken_to_empty_filters()
      {
         _edited = nullptr;
         fill_ui();
      }


      /////////////////////////////////////////////////////////////////////////
      //
      // data.

      const named_filters_t& _known_filters;
      undo_stack& _undo_redo;
      filter_editor_t& _editor;
      tree_filter_ptr_t _edited;
      wstring _filter_name;

      tree_filter_list_widget_t* _filter_list;
      QWidgetScrollListWidget* _filter_scroll;

      int _disable_feedback = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   filter_editor_t::filter_editor_t(const named_filters_t& known, undo_stack& undoRedo, QWidget* parent)
   : QWidget(parent), _ui(make_unique<filters_editor_ui>(known, undoRedo, *this))
   {
   }

   void filter_editor_t::set_edited(const tree_filter_ptr_t& edited, const wstring& name, bool forced)
   {
      if (!_ui)
         return;

      _ui->set_edited(edited, name, forced);
   }

   tree_filter_ptr_t filter_editor_t::get_edited() const
   {
      if (!_ui)
         return {};

      return _ui->get_edited();
   }

   wstring filter_editor_t::get_edited_name() const
   {
      if (!_ui)
         return {};

      return _ui->get_edited_name();
   }
}

// vim: sw=3 : sts=3 : et : sta : 
