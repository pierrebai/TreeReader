#include "NamedFilters.h"

#include <QtCore/qabstractitemmodel.h>

#include <memory>

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;
   using NamedFilters = TreeReader::NamedFilters;

   struct KnownFiltersModel : QAbstractListModel
   {
      std::shared_ptr<NamedFilters> KnownFilters = std::make_shared<NamedFilters>();
      std::vector<TreeFilterPtr> BaseFilters;

      // Basic model support.

      QVariant data(const QModelIndex& index, int role) const override;
      QVariant headerData(int section, Qt::Orientation orientation, int role) const;
      int rowCount(const QModelIndex& parent = QModelIndex()) const override;

      // Drag support.

      Qt::ItemFlags flags(const QModelIndex& index) const override;
      Qt::DropActions supportedDragActions() const;
      QStringList mimeTypes() const override;
      QMimeData* mimeData(const QModelIndexList& indexes) const override;

   private:
      TreeFilterPtr GetFilterAt(const QModelIndex& index) const;
      TreeFilterPtr GetFilterAt(int row) const;
   };
}

