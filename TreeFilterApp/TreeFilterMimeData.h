#include "TreeFilter.h"

#include <QtCore/qmimedata.h>

#include <vector>

namespace TreeReaderApp
{
   struct TreeFilterMimeData : QMimeData
   {
      std::vector<TreeReader::TreeFilterPtr> Filters;

      static constexpr char MimeType[] = "application/x-tree-reader-tree-filter";
   };
}

