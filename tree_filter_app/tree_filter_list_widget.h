#pragma once

#include "dak/tree_reader/tree_filter.h"
#include "dak/QtAdditions/QWidgetListWidget.h"
#include "TreeFilterListItem.h"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QVBoxLayout;

namespace dak::tree_reader::app
{
   using tree_filter_ptr = tree_reader::tree_filter_ptr;
   using QWidgetListWidget = Qtadditions::QWidgetListWidget;

   /////////////////////////////////////////////////////////////////////////
   //
   // Widget containing a list of filters.

   struct TreeFilterListWidget : public QWidgetListWidget
   {
      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      using DeleteCallbackfunction = TreeFilterListItem::DeleteCallbackfunction;
      using EditCallbackfunction = TreeFilterListItem::EditCallbackfunction;

      DeleteCallbackfunction DeleteCallback;
      EditCallbackfunction EditCallback;

      // Create a tree filter drag widget.
      TreeFilterListWidget(
         DeleteCallbackfunction delCallback = {},
         EditCallbackfunction editCallback = {},
         ListModifiedCallbackfunction modifCallback = {},
         bool stretch = true, QWidget* parent = nullptr);

      // add a filter panel UI to the given list panel, with an optional deletion callback.
      QWidgetListItem* addTreeFilter(const tree_filter_ptr& filter, int index = -1);
      QWidgetListItem* addTreeFilter(const tree_filter_ptr& filter, DeleteCallbackfunction delCallback, EditCallbackfunction editCallback, int index = -1);

      // Retrieve all filters kept directly in the list.
      std::vector<tree_filter_ptr> getTreeFilters() const;

   protected:
      QWidgetListItem* cloneItem(QWidgetListItem*) const override;
   };
}
