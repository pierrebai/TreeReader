#pragma once

#include "TreeFilter.h"
#include "TreeFilterWidget.h"
#include "QWidgetListWidget.h"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QVBoxLayout;

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;
   using QWidgetListWidget = QtAdditions::QWidgetListWidget;

   struct TreeFilterListWidget : public QWidgetListWidget
   {
      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      using CallbackFunction = std::function<void(TreeFilterWidget * panel)> ;
      using DeleteCallbackFunction = CallbackFunction;
      using EditCallbackFunction = CallbackFunction;

      DeleteCallbackFunction DeleteCallback;
      EditCallbackFunction EditCallback;

      // Create a tree filter list widget.
      TreeFilterListWidget(DeleteCallbackFunction delCallback = {}, EditCallbackFunction editCallback = {}, QWidget* parent = nullptr);

      // Add a filter panel UI to the given list panel, with an optional deletion callback.
      QWidgetListItem* AddTreeFilter(const TreeFilterPtr& filter, int index = -1);
      QWidgetListItem* AddTreeFilter(const TreeFilterPtr& filter, DeleteCallbackFunction delCallback, EditCallbackFunction editCallback, int index = -1);

      // Retrieve all filters kept in the list.
      std::vector<TreeFilterPtr> GetTreeFilters() const;

   protected:
      QWidgetListItem* CloneItem(QWidgetListItem*) const override;
   };
}
