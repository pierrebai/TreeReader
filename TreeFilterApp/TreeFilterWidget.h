#pragma once

#include "TreeFilter.h"

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qscrollarea.h>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

   struct TreeFilterWidget : QWidget
   {
      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      typedef std::function<void(const TreeFilterPtr & filter, QWidget * panel)> DeleteCallbackFunction;

      TreeFilterWidget(
         const TreeFilterPtr& filter,
         DeleteCallbackFunction delFunc,
         const wchar_t* textContent = nullptr,
         const bool* includeSelf = nullptr,
         const size_t* count = nullptr,
         const size_t* count2 = nullptr);
   };
}
