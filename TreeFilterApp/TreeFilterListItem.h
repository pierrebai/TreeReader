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

   struct TreeFilterListWidget;

   /////////////////////////////////////////////////////////////////////////
   //
   // Item in the tree filter list widget. contains a tree filter.

   struct TreeFilterListItem : QWidgetListItem
   {
      tree_filter_ptr Filter;
      TreeFilterListWidget* SubList = nullptr;

      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      using Callbackfunction = std::function<void(TreeFilterListItem * item)>;
      using DeleteCallbackfunction = Callbackfunction;
      using EditCallbackfunction = Callbackfunction;

      TreeFilterListItem(
         const tree_filter_ptr& filter,
         DeleteCallbackfunction delfunc,
         EditCallbackfunction editfunc,
         std::wstring* textContent = nullptr,
         bool* includeSelf = nullptr,
         size_t* count = nullptr,
         size_t* count2 = nullptr);

      // Create a filter panel UI, with an optional deletion callback.
      static TreeFilterListItem* Create(
         const tree_filter_ptr& filter,
         DeleteCallbackfunction delfunc,
         EditCallbackfunction editfunc);

      TreeFilterListItem* clone() const override;
      TreeFilterListItem* clone(DeleteCallbackfunction delfunc, EditCallbackfunction editfunc) const;

      Q_OBJECT;
   };
}
