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

   struct TreeFilterWidget : QWidgetListItem
   {
      TreeFilterPtr Filter;

      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      typedef std::function<void(QWidgetListItem * panel)> DeleteCallbackFunction;

      TreeFilterWidget(
         const TreeFilterPtr& filter,
         DeleteCallbackFunction delFunc,
         std::wstring* textContent = nullptr,
         bool* includeSelf = nullptr,
         size_t* count = nullptr,
         size_t* count2 = nullptr);

      // Create a filter panel UI, with an optional deletion callback.
      static TreeFilterWidget* Create(const TreeFilterPtr& filter, DeleteCallbackFunction delFunc);

      TreeFilterWidget* Clone() const override;
      TreeFilterWidget* Clone(DeleteCallbackFunction delFunc) const;
   };
}
