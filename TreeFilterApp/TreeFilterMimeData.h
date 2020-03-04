#pragma once

#include "TreeFilter.h"

#include <QtCore/qmimedata.h>

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;
   struct TreeFilterWidget;

   struct TreeFilterMimeData : public QMimeData
   {
      TreeFilterWidget* Widget = nullptr;
      QPoint HotSpot;
   };
}
