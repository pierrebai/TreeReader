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

   struct TreeFilterPanel : public QScrollArea
   {
      TreeFilterPanel(QWidget* parent = nullptr);

      // Clears the list panel of all filters.
      void Clear();

      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      typedef std::function<void(const TreeFilterPtr & filter, QWidget * panel)> DeleteCallbackFunction;

      // Add a filter panel UI to the given list panel, with an optional deletion callback.
      void AddTreeFilterPanel(const TreeFilterPtr& filter, DeleteCallbackFunction = {});

   protected:
      void dragEnterEvent(QDragEnterEvent* event) override;
      void dragLeaveEvent(QDragLeaveEvent* event) override;
      void dragMoveEvent(QDragMoveEvent* event) override;
      void dropEvent(QDropEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;

      QRect _highlightedRect;
   };
}
