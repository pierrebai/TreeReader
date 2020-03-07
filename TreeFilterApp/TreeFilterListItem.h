#pragma once

#include "TreeFilter.h"
#include "QWidgetListItem.h"

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qscrollarea.h>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;
   using QWidgetListItem = QtAdditions::QWidgetListItem;

   struct TreeFilterListWidget;

   struct TreeFilterListItem : QWidgetListItem
   {
      TreeFilterPtr Filter;
      TreeFilterListWidget* SubList = nullptr;

      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      using CallbackFunction = std::function<void(TreeFilterListItem * panel)>;
      using DeleteCallbackFunction = CallbackFunction;
      using EditCallbackFunction = CallbackFunction;

      TreeFilterListItem(
         const TreeFilterPtr& filter,
         DeleteCallbackFunction delFunc,
         EditCallbackFunction editFunc,
         std::wstring* textContent = nullptr,
         bool* includeSelf = nullptr,
         size_t* count = nullptr,
         size_t* count2 = nullptr);

      // Create a filter panel UI, with an optional deletion callback.
      static TreeFilterListItem* Create(
         const TreeFilterPtr& filter,
         DeleteCallbackFunction delFunc,
         EditCallbackFunction editFunc);

      TreeFilterListItem* Clone() const override;
      TreeFilterListItem* Clone(DeleteCallbackFunction delFunc, EditCallbackFunction editFunc) const;
   };
}
