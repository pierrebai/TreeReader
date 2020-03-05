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
      typedef std::function<void(TreeFilterWidget * panel)> DeleteCallbackFunction;

      DeleteCallbackFunction DeleteCallback;

      // Create a tree filter list widget.
      TreeFilterListWidget(DeleteCallbackFunction callback = {}, QWidget* parent = nullptr);

      // Add a filter panel UI to the given list panel, with an optional deletion callback.
      QWidgetListItem* AddTreeFilter(const TreeFilterPtr& filter, int index = -1);
      QWidgetListItem* AddTreeFilter(const TreeFilterPtr& filter, DeleteCallbackFunction callback, int index = -1);

      // Retrieve all filters kept in the list.
      std::vector<TreeFilterPtr> GetTreeFilters() const;

   protected:
      QWidgetListItem* CloneItem(QWidgetListItem*) const override;
   };
}
