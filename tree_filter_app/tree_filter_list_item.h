#pragma once

#include "dak/tree_reader/tree_filter.h"
#include "dak/QtAdditions/QWidgetListItem.h"

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qscrollarea.h>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

namespace dak::tree_reader::app
{
   using tree_filter_ptr_t = tree_reader::tree_filter_ptr_t;
   using QWidgetListItem = QtAdditions::QWidgetListItem;

   struct tree_filter_list_widget_t;

   /////////////////////////////////////////////////////////////////////////
   //
   // Item in the tree filter list widget. contains a tree filter.

   struct tree_filter_list_item_t : QWidgetListItem
   {
      tree_filter_ptr_t filter;
      tree_filter_list_widget_t* sub_list = nullptr;

      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      using callback_function_t = std::function<void(tree_filter_list_item_t * item)>;
      using delete_callback_function_t = callback_function_t;
      using edit_callback_function_t = callback_function_t;

      tree_filter_list_item_t(
         const tree_filter_ptr_t& filter,
         delete_callback_function_t delfunc,
         edit_callback_function_t editfunc,
         std::wstring* textContent = nullptr,
         bool* includeSelf = nullptr,
         size_t* count = nullptr,
         size_t* count2 = nullptr);

      // Create a filter panel UI, with an optional deletion callback.
      static tree_filter_list_item_t* create(
         const tree_filter_ptr_t& filter,
         delete_callback_function_t delfunc,
         edit_callback_function_t editfunc);

      tree_filter_list_item_t* clone() const override;
      tree_filter_list_item_t* clone(delete_callback_function_t delfunc, edit_callback_function_t editfunc) const;

      Q_OBJECT;
   };
}
