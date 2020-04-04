#include "TextTreeModel.h"

namespace dak::tree_reader::app
{
   using namespace std;
   using namespace dak::tree_reader;
   using node = text_tree::node;

   void TextTreeModel::reset()
   {
      beginResetModel();
      endResetModel();
   }

   QVariant TextTreeModel::data(const QModelIndex& index, int role) const
   {
      if (!Tree)
         return QVariant();

      if (role != Qt::DisplayRole)
         return QVariant();

      if (!index.isValid())
         return QVariant();

      const node* a_node = static_cast<node*>(index.internalPointer());
      if (!a_node)
         return QVariant();

      return QVariant(QString::fromWCharArray(a_node->text_ptr));
   }

   QVariant TextTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
   {
      return QVariant();
   }

   QModelIndex TextTreeModel::index(int row, int column, const QModelIndex& parent) const
   {
      if (!Tree)
         return QModelIndex();

      const node* parentnode = parent.isValid() ? static_cast<node*>(parent.internalPointer()) : nullptr;
      const std::vector<node*>& nodes = parentnode ? parentnode->children : Tree->roots;
      if (row < 0 || row >= nodes.size())
         return QModelIndex();

      return createIndex(row, column, static_cast<void*>(nodes[row]));
   }

   QModelIndex TextTreeModel::parent(const QModelIndex& index) const
   {
      if (!Tree)
         return QModelIndex();

      if (!index.isValid())
         return QModelIndex();

      const node* a_node = static_cast<node*>(index.internalPointer());
      if (!a_node)
         return QModelIndex();

      if (!a_node->parent)
         return QModelIndex();

      return createIndex(int(a_node->index_in_parent), 0, static_cast<void*>(a_node->parent));
   }

   int TextTreeModel::rowCount(const QModelIndex& parent) const
   {
      if (!Tree)
         return 0;

      if (!parent.isValid())
         return int(Tree->roots.size());

      const node* a_node = static_cast<node*>(parent.internalPointer());
      if (!a_node)
         return 0;

      return int(a_node->children.size());
   }
   int TextTreeModel::columnCount(const QModelIndex& parent) const
   {
      if (!Tree)
         return 0;
      return 1;
   }
}
