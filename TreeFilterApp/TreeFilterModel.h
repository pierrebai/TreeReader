#include "TreeFilter.h"

#include <QtCore/qabstractitemmodel.h>

#include <memory>

namespace TreeReaderApp
{
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

   struct TreeFilterModel : QAbstractItemModel
   {
      TreeFilterPtr Filter;

      // Basic model support.

      QVariant data(const QModelIndex& index, int role) const override;
      QVariant headerData(int section, Qt::Orientation orientation, int role) const;
      QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
      QModelIndex parent(const QModelIndex& index) const override;
      int rowCount(const QModelIndex& parent = QModelIndex()) const override;
      int columnCount(const QModelIndex& parent = QModelIndex()) const override;

      // Row removal.

      bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

      // Drag and drop support.

      Qt::ItemFlags flags(const QModelIndex& index) const override;
      Qt::DropActions supportedDropActions() const override;
      QStringList mimeTypes() const override;
      QMimeData* mimeData(const QModelIndexList& indexes) const override;
      bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
      bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
   };
}

