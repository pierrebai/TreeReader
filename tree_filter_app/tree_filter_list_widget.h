#pragma once

#include "dak/tree_reader/tree_filter.h"
#include "dak/QtAdditions/QWidgetListWidget.h"
#include "tree_filter_list_item.h"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QVBoxLayout;

namespace dak::tree_reader::app
{
   using tree_filter_ptr_t = tree_reader::tree_filter_ptr_t;
   using QWidgetListWidget = QtAdditions::QWidgetListWidget;

   /////////////////////////////////////////////////////////////////////////
   //
   // Widget containing a list of filters.

   struct tree_filter_list_widget_t : public QWidgetListWidget
   {
      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      using delete_callback_function_t = tree_filter_list_item_t::delete_callback_function_t;
      using edit_callback_function_t = tree_filter_list_item_t::edit_callback_function_t;

      delete_callback_function_t delete_callback;
      edit_callback_function_t edit_callback;

      // Create a tree filter drag widget.
      tree_filter_list_widget_t(
         delete_callback_function_t del_func = {},
         edit_callback_function_t edit_func = {},
         ListModifiedCallbackFunction modif_func = {},
         bool stretch = true, QWidget* parent = nullptr);

      // add a filter panel UI to the given list panel, with an optional deletion callback.
      QWidgetListItem* add_tree_filter(const tree_filter_ptr_t& filter, int index = -1);
      QWidgetListItem* add_tree_filter(const tree_filter_ptr_t& filter, delete_callback_function_t del_func, edit_callback_function_t edit_func, int index = -1);

      // Retrieve all filters kept directly in the list.
      std::vector<tree_filter_ptr_t> get_tree_filters() const;

   protected:
      QWidgetListItem* cloneItem(QWidgetListItem*) const override;
   };
}
