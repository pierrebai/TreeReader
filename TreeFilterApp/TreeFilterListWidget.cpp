#include "TreeFilterListWidget.h"
#include "TreeFilterMimeData.h"
#include "QtUtilities.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qlayout.h>

#include <QtGui/qdrag.h>
#include <QtGui/qpainter.h>
#include <QtGui/qwindow.h>
#include <QtGui/qvalidator.h>

#include <QtGui/qevent.h>

namespace TreeReaderApp
{
   using namespace TreeReader;
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree filter panel.

   TreeFilterListWidget::TreeFilterListWidget(DeleteCallbackFunction callback, QWidget* parent)
   : QScrollArea(parent), DeleteCallback(callback)
   {
      auto availLayout = new QVBoxLayout;
      availLayout->setSizeConstraint(QLayout::SetMinimumSize);
      availLayout->setMargin(2);

      auto availWidget = new QWidget;
      availWidget->setBackgroundRole(QPalette::ColorRole::Base);
      availWidget->setLayout(availLayout);

      setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      setWidget(availWidget);
      setWidgetResizable(true);
      setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
   }

   void TreeFilterListWidget::Clear()
   {
      auto availWidget = widget();
      if (!availWidget)
         return;

      for (auto child : children())
         if (!dynamic_cast<QLayout*>(child))
            delete child;
   }

   void TreeFilterListWidget::AddTreeFilter(const TreeFilterPtr& filter)
   {
      AddTreeFilter(TreeFilterWidget::Create(filter, DeleteCallback));
   }

   void TreeFilterListWidget::AddTreeFilter(TreeFilterWidget* filter)
   {
      if (!filter)
         return;

      auto availWidget = widget();
      if (!availWidget)
         return;

      auto layout = availWidget->layout();
      if (!layout)
         return;

      layout->addWidget(filter);

      availWidget->setMinimumWidth(max(availWidget->minimumWidth(), filter->sizeHint().width()));
      setMinimumWidth(max(minimumWidth(), filter->sizeHint().width()));
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Drag and drop.

   static constexpr char TreeFilterMimeType[] = "application/x-tree-reader-tree-filter";

   void TreeFilterListWidget::dragEnterEvent(QDragEnterEvent* event)
   {
      const TreeFilterMimeData* mime = dynamic_cast<const TreeFilterMimeData*>(event->mimeData());
      if (!mime)
         return event->ignore();

      if (event->source() == this)
      {
         event->setDropAction(Qt::MoveAction);
         event->accept();
      }
      else
      {
         event->acceptProposedAction();
      }
   }

   void TreeFilterListWidget::dragLeaveEvent(QDragLeaveEvent* event)
   {
      event->accept();
   }

   void TreeFilterListWidget::dragMoveEvent(QDragMoveEvent* event)
   {
      const TreeFilterMimeData* mime = dynamic_cast<const TreeFilterMimeData*>(event->mimeData());
      if (!mime)
         return event->ignore();

      event->setDropAction(Qt::MoveAction);
      event->accept();
   }

   void TreeFilterListWidget::dropEvent(QDropEvent* event)
   {
      const TreeFilterMimeData* mime = dynamic_cast<const TreeFilterMimeData*>(event->mimeData());
      if (!mime)
         return event->ignore();

      const QPoint position = event->pos();
      TreeFilterWidget* dropOn = FindWidgetAt(position);
      const bool dropAbove = dropOn ? (position.y() < dropOn->pos().y() + dropOn->size().height() / 2) : false;

      if (event->source() == this)
      {
         // Remove panel and insert it at correct position in list.
         auto movedWidget = mime->Widget;
         if (movedWidget)
         {
            movedWidget->setParent(nullptr);
            // TODO: find right spot to insert.
            AddTreeFilter(movedWidget);
         }
         event->setDropAction(Qt::MoveAction);
         event->accept();
      }
      else
      {
         auto newWidget = mime->Widget->Clone(DeleteCallback);
         if (newWidget)
         {
            // TODO: find right spot to insert.
            AddTreeFilter(newWidget);
         }
         event->acceptProposedAction();
      }
   }

   void TreeFilterListWidget::mousePressEvent(QMouseEvent* event)
   {
      TreeFilterWidget* widget = FindWidgetAt(event->pos());
      if (!widget)
         return;

      TreeFilterMimeData* mimeData = new TreeFilterMimeData;
      mimeData->Widget = widget;
      mimeData->HotSpot = event->pos() - widget->pos();
      mimeData->setData(TreeFilterMimeType, QByteArray());

      qreal dpr = window()->windowHandle()->devicePixelRatio();
      QPixmap pixmap(widget->size() * dpr);
      pixmap.setDevicePixelRatio(dpr);
      widget->render(&pixmap);

      QDrag* drag = new QDrag(this);
      drag->setMimeData(mimeData);
      drag->setPixmap(pixmap);
      drag->setHotSpot(mimeData->HotSpot);

      Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
   }

   TreeFilterWidget* TreeFilterListWidget::FindWidgetAt(const QPoint& pt) const
   {
      for (auto child = childAt(pt); child; child = child->parentWidget())
         if (auto widget = dynamic_cast<TreeFilterWidget*>(child))
            return widget;

      return nullptr;
   }

}

