#pragma once

#include "TreeFilter.h"
#include "TreeFilterListItem.h"
#include "QWidgetListWidget.h"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QVBoxLayout;

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;
   using QWidgetListWidget = QtAdditions::QWidgetListWidget;

   /////////////////////////////////////////////////////////////////////////
   //
   // Widget containing a list of filters.

   struct TreeFilterListWidget : public QWidgetListWidget
   {
      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      using DeleteCallbackFunction = TreeFilterListItem::DeleteCallbackFunction;
      using EditCallbackFunction = TreeFilterListItem::EditCallbackFunction;

      DeleteCallbackFunction DeleteCallback;
      EditCallbackFunction EditCallback;

      // Create a tree filter drag widget.
      TreeFilterListWidget(
         DeleteCallbackFunction delCallback = {},
         EditCallbackFunction editCallback = {},
         ListModifiedCallbackFunction modifCallback = {},
         bool stretch = true, QWidget* parent = nullptr);

      // Add a filter panel UI to the given list panel, with an optional deletion callback.
      QWidgetListItem* AddTreeFilter(const TreeFilterPtr& filter, int index = -1);
      QWidgetListItem* AddTreeFilter(const TreeFilterPtr& filter, DeleteCallbackFunction delCallback, EditCallbackFunction editCallback, int index = -1);

      // Retrieve all filters kept directly in the list.
      std::vector<TreeFilterPtr> GetTreeFilters() const;

   protected:
      QWidgetListItem* CloneItem(QWidgetListItem*) const override;
   };
}
