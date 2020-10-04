#include "text_tree_model.h"

namespace dak::tree_reader::app
{
   using namespace std;
   using namespace dak::tree_reader;
   using node = text_tree_t::node_t;

   void text_tree_model_t::reset()
   {
      beginResetModel();
      endResetModel();
   }

   QVariant text_tree_model_t::data(const QModelIndex& index, int role) const
   {
      if (!tree)
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

   QVariant text_tree_model_t::headerData(int section, Qt::Orientation orientation, int role) const
   {
      return QVariant();
   }

   QModelIndex text_tree_model_t::index(int row, int column, const QModelIndex& parent) const
   {
      if (!tree)
         return QModelIndex();

      const node* parent_node = parent.isValid() ? static_cast<node*>(parent.internalPointer()) : nullptr;
      const std::vector<node*>& nodes = parent_node ? parent_node->children : tree->roots;
      if (row < 0 || row >= nodes.size())
         return QModelIndex();

      return createIndex(row, column, static_cast<void*>(nodes[row]));
   }

   QModelIndex text_tree_model_t::parent(const QModelIndex& index) const
   {
      if (!tree)
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

   int text_tree_model_t::rowCount(const QModelIndex& parent) const
   {
      if (!tree)
         return 0;

      if (!parent.isValid())
         return int(tree->roots.size());

      const node* a_node = static_cast<node*>(parent.internalPointer());
      if (!a_node)
         return 0;

      return int(a_node->children.size());
   }
   int text_tree_model_t::columnCount(const QModelIndex& parent) const
   {
      if (!tree)
         return 0;
      return 1;
   }
}
