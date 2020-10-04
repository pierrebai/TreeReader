#include "dak/tree_reader/text_tree.h"

#include <QtCore/qabstractitemmodel.h>

#include <memory>

namespace dak::tree_reader::app
{
   using text_tree_ptr_t = tree_reader::text_tree_ptr_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Tree model containing all lines of a text tree.

   struct text_tree_model_t : QAbstractItemModel
   {
      text_tree_ptr_t tree;

      void reset();

      QVariant data(const QModelIndex& index, int role) const override;
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
      QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
      QModelIndex parent(const QModelIndex& index) const override;
      int rowCount(const QModelIndex& parent = QModelIndex()) const override;
      int columnCount(const QModelIndex& parent = QModelIndex()) const override;
   };
}

