#pragma once

#include "QWidgetListItem.h"

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qscrollarea.h>

#include <functional>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QVBoxLayout;

namespace QtAdditions
{
   struct QWidgetListWidget : public QScrollArea
   {
      // Callback signature when the user wants to delete a filter.
      // Deleting the panel removes it from the list panel that contains it.
      typedef std::function<void(QWidgetListItem * panel)> DeleteCallbackFunction;

      DeleteCallbackFunction DeleteCallback;

      // Create a widget list widget.
      QWidgetListWidget(DeleteCallbackFunction callback = {}, QWidget* parent = nullptr);

      // Clears the list panel of all filters.
      void Clear();

      // Add a widget item, with an optional deletion callback.
      QWidgetListItem* AddItem(QWidgetListItem* item, int index = -1);

      // Remove a widget item.
      void RemoveItem(QWidgetListItem* item);

   protected:
      virtual QWidgetListItem* CloneItem(QWidgetListItem*) const;

      void dragEnterEvent(QDragEnterEvent* event) override;
      void dragLeaveEvent(QDragLeaveEvent* event) override;
      void dragMoveEvent(QDragMoveEvent* event) override;
      void dropEvent(QDropEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;

      QWidgetListItem* FindWidgetAt(const QPoint& pt) const;

      QVBoxLayout* _layout = nullptr;
   };
}
