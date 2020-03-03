#include "TreeFilterItem.h"

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

   void AddTreeFilterItem(QListWidget* list, const TreeFilterPtr& filter)
   {
      if (!list)
         return;

      auto item = CreateTreeFilterItem(filter);
      if (!item)
         return;

      list->addItem(item);
   }
}

// vim: sw=3 : sts=3 : et : sta : 
