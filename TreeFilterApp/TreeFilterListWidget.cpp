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
      _availLayout = new QVBoxLayout;
      _availLayout->setSizeConstraint(QLayout::SetMinimumSize);
      _availLayout->setMargin(2);
      _availLayout->addStretch();

      auto availWidget = new QWidget;
      availWidget->setBackgroundRole(QPalette::ColorRole::Base);
      availWidget->setLayout(_availLayout);

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

   TreeFilterWidget* TreeFilterListWidget::AddTreeFilter(const TreeFilterPtr& filter, int index)
   {
      return AddTreeFilter(TreeFilterWidget::Create(filter, DeleteCallback), index);
   }

   TreeFilterWidget* TreeFilterListWidget::AddTreeFilter(TreeFilterWidget* filter, int index)
   {
      if (!filter)
         return nullptr;

      auto availWidget = widget();
      if (!availWidget)
         return nullptr;

      if (!_availLayout)
         return nullptr;

      if (index < 0 || index >= _availLayout->count())
         index = _availLayout->count() - 1;

      _availLayout->insertWidget(index, filter);

      availWidget->setMinimumWidth(max(availWidget->minimumWidth(), filter->sizeHint().width()));
      setMinimumWidth(max(minimumWidth(), filter->sizeHint().width()));

      return filter;
   }

   void TreeFilterListWidget::RemoveTreeFilter(TreeFilterWidget* filter)
   {
      filter->setParent(nullptr);
      _availLayout->removeWidget(filter);
      _availLayout->update();
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
      const int dropIndexOffset = dropAbove ? 0 : 1;
      const int dropOnIndex = dropOn ? _availLayout->indexOf(dropOn) : -1000;

      if (event->source() == this)
      {
         // Remove panel and insert it at correct position in list.
         auto movedWidget = mime->Widget;
         if (movedWidget && movedWidget != dropOn)
         {
            const int movedIndex = _availLayout->indexOf(movedWidget);
            const int newIndex = dropOnIndex + dropIndexOffset - (movedIndex < dropOnIndex ? 1 : 0);
            RemoveTreeFilter(movedWidget);
            AddTreeFilter(movedWidget, newIndex);
         }
         event->setDropAction(Qt::MoveAction);
         event->accept();
      }
      else
      {
         auto newWidget = mime->Widget->Clone(DeleteCallback);
         if (newWidget)
         {
            const int newIndex = dropOnIndex + dropIndexOffset;
            AddTreeFilter(newWidget, newIndex);
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

