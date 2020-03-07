#pragma once

#include <QtWidgets/qscrollarea.h>

class QVBoxLayout;
class QWidget;

namespace QtAdditions
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Scrollable list of widgets.

   struct QWidgetScrollListWidget : public QScrollArea
   {
      // Create a scrollable widget list widget.
      QWidgetScrollListWidget(QWidget * widget, QWidget* parent = nullptr);
   };
}
