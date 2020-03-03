#pragma once

#include "TreeFilter.h"

#include <QtWidgets/qlistwidget.h>

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

   QListWidgetItem* CreateTreeFilterItem(const TreeFilterPtr& filter);
   void AddTreeFilterItem(QListWidget* list, const TreeFilterPtr& filter);
}

// vim: sw=3 : sts=3 : et : sta : 
