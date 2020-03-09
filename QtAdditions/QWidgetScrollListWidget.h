#pragma once

#include <QtWidgets/qscrollarea.h>

class QWidget;

namespace QtAdditions
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Scrollable widget.

   struct QWidgetScrollListWidget : public QScrollArea
   {
      // Create a scrollable widget.
      QWidgetScrollListWidget(QWidget * widget, QWidget* parent = nullptr);
   };
}
