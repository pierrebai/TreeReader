#include "TreeFilterListWidget.h"
#include "TreeFilterWidget.h"
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

   namespace
   {
      using DeleteCallbackFunction = TreeFilterListWidget::DeleteCallbackFunction;

      namespace L
      {
         inline const wchar_t* t(const wchar_t* text)
         {
            return text;
         }
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // Filter panel creation helpers.

      QWidget* CreateFilterPanel(const shared_ptr<AcceptTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<StopTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<UntilTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<ContainsTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, filter->Contained.c_str());
      }

      QWidget* CreateFilterPanel(const shared_ptr<RegexTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, filter->RegexTextForm.c_str());
      }

      QWidget* CreateFilterPanel(const shared_ptr<NotTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<IfSubTreeTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<IfSiblingTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<CountChildrenTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr, &filter->IncludeSelf, &filter->Count);
      }

      QWidget* CreateFilterPanel(const shared_ptr<CountSiblingsTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr, &filter->IncludeSelf, &filter->Count);
      }

      QWidget* CreateFilterPanel(const shared_ptr<OrTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<AndTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<UnderTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr, &filter->IncludeSelf);
      }

      QWidget* CreateFilterPanel(const shared_ptr<RemoveChildrenTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr, &filter->IncludeSelf);
      }

      QWidget* CreateFilterPanel(const shared_ptr<LevelRangeTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr, nullptr, &filter->MinLevel, &filter->MaxLevel);
      }

      QWidget* CreateFilterPanel(const shared_ptr<NamedTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return new TreeFilterWidget(filter, delFunc, nullptr);
      }

      QWidget* ConvertFilterToPanel(const TreeFilterPtr& filter, DeleteCallbackFunction delFunc)
      {
         #define CALL_CONVERTER(a) if (auto ptr = dynamic_pointer_cast<a>(filter)) { return CreateFilterPanel(ptr, delFunc); }

         CALL_CONVERTER(AcceptTreeFilter)
         CALL_CONVERTER(StopTreeFilter)
         CALL_CONVERTER(UntilTreeFilter)
         CALL_CONVERTER(ContainsTreeFilter)
         CALL_CONVERTER(RegexTreeFilter)
         CALL_CONVERTER(NotTreeFilter)
         CALL_CONVERTER(IfSubTreeTreeFilter)
         CALL_CONVERTER(IfSiblingTreeFilter)
         CALL_CONVERTER(CountChildrenTreeFilter)
         CALL_CONVERTER(CountSiblingsTreeFilter)
         CALL_CONVERTER(OrTreeFilter)
         CALL_CONVERTER(AndTreeFilter)
         CALL_CONVERTER(UnderTreeFilter)
         CALL_CONVERTER(RemoveChildrenTreeFilter)
         CALL_CONVERTER(LevelRangeTreeFilter)
         CALL_CONVERTER(NamedTreeFilter)

         #undef CALL_CONVERTER

         return nullptr;
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree filter panel.

   TreeFilterListWidget::TreeFilterListWidget(QWidget* parent)
      : QScrollArea(parent)
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

   void TreeFilterListWidget::AddTreeFilterListWidget(const TreeFilterPtr& filter, DeleteCallbackFunction delFunc)
   {
      auto availWidget = widget();
      if (!availWidget)
         return;

      auto layout = availWidget->layout();
      if (!layout)
         return;

      auto widget = ConvertFilterToPanel(filter, delFunc);
      if (!widget)
         return;

      layout->addWidget(widget);

      availWidget->setMinimumWidth(max(availWidget->minimumWidth(), widget->sizeHint().width()));
      setMinimumWidth(max(minimumWidth(), widget->sizeHint().width()));
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

      QPoint position = event->pos();

      // TODO insert panel.

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

   void TreeFilterListWidget::mousePressEvent(QMouseEvent* event)
   {
      TreeFilterWidget* widget = nullptr;
      for (auto child = childAt(event->pos()); !widget && child; child = child->parentWidget())
         widget = dynamic_cast<TreeFilterWidget*>(child);
      if (!widget)
         return;

      TreeFilterMimeData* mimeData = new TreeFilterMimeData;
      mimeData->Widget = widget;
      mimeData->setData(TreeFilterMimeType, QByteArray());
      mimeData->HotSpot = event->pos() - widget->pos();

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
}

