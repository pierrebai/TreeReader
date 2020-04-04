#pragma once

#include "QWidgetListItem.h"

#include <QtWidgets/qframe.h>
#include <QtWidgets/qlabel.h>

#include <functional>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QVBoxLayout;

namespace Qtadditions
{
   /////////////////////////////////////////////////////////////////////////
   //
   // List widget that contains complex widgets.

   struct QWidgetListWidget : public QFrame
   {
      // Callback signature when the list was modified: added or removed an item.
      using ListModifiedCallbackfunction = std::function<void(QWidgetListWidget * list)>;;

      // Create a widget list widget.
      QWidgetListWidget(ListModifiedCallbackfunction modif_func = {}, bool stretch = true, QWidget * parent = nullptr);

      // Clears the list panel of all items.
      void clear();

      // add a widget item.
      QWidgetListItem* addItem(QWidgetListItem* item, int index = -1);

      // remove a widget item.
      void removeItem(QWidgetListItem* item);

      // Retrieve all widget items kept directly in this list widget.
      std::vector<QWidgetListItem*> getItems() const;

   protected:
      virtual QWidgetListItem* cloneItem(QWidgetListItem*) const;

      void dragEnterEvent(QDragEnterEvent* event) override;
      void dragLeaveEvent(QDragLeaveEvent* event) override;
      void dragMoveEvent(QDragMoveEvent* event) override;
      void dropEvent(QDropEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;
      void childEvent(QChildEvent* event) override;

      QWidgetListItem* FindWidgetAt(const QPoint& pt) const;

      void UpdateDropHereLabel();
      void PropagateMinimumWidth();

      ListModifiedCallbackfunction _modifCallback;
      QVBoxLayout* _layout = nullptr;
      QLabel* _dropHere = nullptr;

      Q_OBJECT;
   };
}
