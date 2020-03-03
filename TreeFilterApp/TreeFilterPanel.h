#pragma once

#include "TreeFilter.h"

#include <QtWidgets/qwidget.h>

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

   QWidget* ConvertFilterToPanel(const TreeFilterPtr& filter);
   void AddTreeFilterPanel(QLayout* list, const TreeFilterPtr& filter);
}
