#pragma once

#include "TreeFilter.h"

#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qstyleditemdelegate.h>

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

   QListWidgetItem* CreateTreeFilterItem(const TreeFilterPtr& filter);
   void AddTreeFilterItem(QListWidget* list, const TreeFilterPtr& filter);

   struct TreeFilterItemDelegate : QStyledItemDelegate
   {
      void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
      QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
