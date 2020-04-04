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
   using tree_filter_ptr = tree_reader::tree_filter_ptr;
   using QWidgetListItem = Qtadditions::QWidgetListItem;

   struct tree_filter_list_widget;

   /////////////////////////////////////////////////////////////////////////
   //
   // Item in the tree filter list widget. contains a tree filter.

   struct tree_filter_list_item : QWidgetListItem
   {
      tree_filter_ptr filter;
      tree_filter_list_widget* sub_list = nullptr;

      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      using callback_function = std::function<void(tree_filter_list_item * item)>;
      using delete_callback_function = callback_function;
      using edit_callback_function = callback_function;

      tree_filter_list_item(
         const tree_filter_ptr& filter,
         delete_callback_function delfunc,
         edit_callback_function editfunc,
         std::wstring* textContent = nullptr,
         bool* includeSelf = nullptr,
         size_t* count = nullptr,
         size_t* count2 = nullptr);

      // Create a filter panel UI, with an optional deletion callback.
      static tree_filter_list_item* create(
         const tree_filter_ptr& filter,
         delete_callback_function delfunc,
         edit_callback_function editfunc);

      tree_filter_list_item* clone() const override;
      tree_filter_list_item* clone(delete_callback_function delfunc, edit_callback_function editfunc) const;

      Q_OBJECT;
   };
}
