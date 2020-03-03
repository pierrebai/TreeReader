#include "TreeFilterItem.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>

namespace TreeReaderApp
{
   static constexpr int FilterTypeRole          = Qt::UserRole + 0;
   static constexpr int FilterTextRole          = Qt::UserRole + 1;
   static constexpr int FilterMinLevelRole      = Qt::UserRole + 2;
   static constexpr int FilterMaxLevelRole      = Qt::UserRole + 3;
   static constexpr int FilterIncludeSelfRole   = Qt::UserRole + 4;
   static constexpr int FilterAddressRole       = Qt::UserRole + 5;
   static constexpr int FilterCountRole         = Qt::UserRole + 6;

   QListWidgetItem* CreateTreeFilterItem(const TreeFilterPtr& filter)
   {
      if (!filter)
         return nullptr;

      auto item = new QListWidgetItem(QString::fromStdWString(filter->GetName()));

      item->setToolTip(QString::fromStdWString(filter->GetDescription()));

      return item;
   }

   QWidget* CreateTreeFilterWidget(const TreeFilterPtr& filter)
   {
      QWidget* container = new QWidget();
      QVBoxLayout* layout = new QVBoxLayout(container);
      QString text = QString::fromStdWString(filter->GetName());
      QLabel* textLabel = new QLabel(text);
      layout->addWidget(textLabel);
      layout->addSpacing(50);
      layout->setSizeConstraint(QLayout::SetFixedSize);
      container->setLayout(layout);
      return container;
   }

   void AddTreeFilterItem(QListWidget* list, const TreeFilterPtr& filter)
   {
      if (!list)
         return;

      auto item = CreateTreeFilterItem(filter);
      if (!item)
         return;

      //auto widget = CreateTreeFilterWidget(filter);
      //item->setSizeHint(widget->sizeHint());

      list->addItem(item);
      //list->setItemWidget(item, widget);
   }

   //void TreeFilterItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
   //{
   //   QStyleOption
   //   QStyleOptionFrameV3 frame;
   //   frame.fontMetrics = option.fontMetrics;
   //   frame;
   //}

   //QSize TreeFilterItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
   //{
   //   QString text = index.data(Qt::DisplayRole).value<QString>();
   //   QRect textRect = option.fontMetrics.tightBoundingRect(text);
   //   return textRect.size();
   //}

}

// vim: sw=3 : sts=3 : et : sta : 
