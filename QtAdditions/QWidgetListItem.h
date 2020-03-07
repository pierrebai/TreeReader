#pragma once

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qscrollarea.h>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

namespace QtAdditions
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Complex widget kept in a list widget.

   struct QWidgetListItem : QWidget
   {
      QWidgetListItem(QWidget* parent = nullptr) : QWidget(parent) {}

      virtual QWidgetListItem* Clone() const;
   };
}
