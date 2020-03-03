#pragma once

#include "TreeFilter.h"

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qscrollarea.h>

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

   // Creates a list panel that can contain filter panels.
   QScrollArea* CreateTreeFilterList();

   // Clears the list panel of all filters.
   void ClearTreeFilterList(QScrollArea* list);

   // Callback signature when the user wants to delete a filter.
   // Deleting the panel removes it from the list panel that contains it.
   typedef std::function<void(const TreeFilterPtr & filter, QWidget* panel)> DeleteCallbackFunction;

   // Convert a filter to a panel UI, with an optional deletion callback.
   QWidget* ConvertFilterToPanel(const TreeFilterPtr& filter, DeleteCallbackFunction = {});

   // Add a filter panel UI to the given list panel, with an optional deletion callback.
   void AddTreeFilterPanel(QScrollArea* list, const TreeFilterPtr& filter, DeleteCallbackFunction = {});
}
