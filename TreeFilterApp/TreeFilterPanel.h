#pragma once

#include "TreeFilter.h"

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qscrollarea.h>

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

   QWidget* ConvertFilterToPanel(const TreeFilterPtr& filter);

   QScrollArea* CreateTreeFilterList();
   void ClearTreeFilterList(QScrollArea* list);

   void AddTreeFilterPanel(QScrollArea* list, const TreeFilterPtr& filter);
}
