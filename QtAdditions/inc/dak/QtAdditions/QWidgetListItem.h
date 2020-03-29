#pragma once

#include <QtWidgets/qwidget.h>

namespace Qtadditions
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Complex widget kept in a list widget.

   struct QWidgetListItem : QWidget
   {
      QWidgetListItem(QWidget* parent = nullptr);
      virtual ~QWidgetListItem();

      virtual QWidgetListItem* clone() const;

   protected:
      void enterEvent(QEvent* event) override;
      void leaveEvent(QEvent* event) override;

      void HighlightBackground(bool high);

   };
}
