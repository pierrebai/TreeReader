#include "KnownFiltersModel.h"
#include "TreeFilterHelpers.h"
#include "TreeFilterMimeData.h"

#include <QtCore/qmimedata.h>
#include <QtCore/qdatastream.h>

namespace TreeReaderApp
{
   using namespace std;
   using namespace TreeReader;

   /////////////////////////////////////////////////////////////////////////
   //
   // Internal helpers.

   TreeFilterPtr KnownFiltersModel::GetFilterAt(const QModelIndex& index) const
   {
      if (!index.isValid())
         return {};

      return GetFilterAt(index.row());
   }

   TreeFilterPtr KnownFiltersModel::GetFilterAt(int row) const
   {
      if (row >= 0 && row < BaseFilters.size())
         return BaseFilters[row];

      row -= int(BaseFilters.size());
      if (row >= 0 && size_t(row) < KnownFilters->Filters.size())
      {
         auto pos = KnownFilters->Filters.begin();
         std::advance(pos, row);
         return pos->second;
      }

      return {};
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Basic model support.

   QVariant KnownFiltersModel::data(const QModelIndex& index, int role) const
   {
      auto filter = GetFilterAt(index);
      if (!filter)
         return QVariant();

      wstring text;
      switch (role)
      {
         case Qt::DisplayRole: text = filter->GetName(); break;
         case Qt::ToolTipRole: text = filter->GetDescription(); break;
         default: return QVariant();
      }
      return QVariant(QString::fromStdWString(text));
   }

   QVariant KnownFiltersModel::headerData(int section, Qt::Orientation orientation, int role) const
   {
      return QVariant();
   }

   int KnownFiltersModel::rowCount(const QModelIndex& parent) const
   {
      // Note: valid items have zero rows, only the invalid invisible root item has rows.
      if (parent.isValid())
         return 0;

      return int(BaseFilters.size() + KnownFilters->Filters.size());
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Drag and drop support.

   Qt::ItemFlags KnownFiltersModel::flags(const QModelIndex& index) const
   {
      return Qt::ItemFlag::ItemIsDragEnabled
           | Qt::ItemFlag::ItemIsEnabled
           | Qt::ItemFlag::ItemIsSelectable;
   }

   Qt::DropActions KnownFiltersModel::supportedDragActions() const
   {
      return Qt::CopyAction;
   }

   QStringList KnownFiltersModel::mimeTypes() const
   {
      return QStringList(TreeFilterMimeData::MimeType);
   }

   QMimeData* KnownFiltersModel::mimeData(const QModelIndexList& indexes) const
   {
      TreeFilterMimeData* mimeData = new TreeFilterMimeData;

      for (const QModelIndex& index : indexes)
      {
         auto filter = GetFilterAt(index);
         if (!filter)
            continue;

         mimeData->Filters.push_back(filter);
      }

      return mimeData;
   }
}
