#include "TextTreeModel.h"

namespace TreeReaderApp
{
   using namespace std;
   using namespace TreeReader;
   using Node = TextTree::Node;

   QVariant TextTreeModel::data(const QModelIndex& index, int role) const
   {
      if (!Tree)
         return QVariant();

      if (role != Qt::DisplayRole)
         return QVariant();

      if (!index.isValid())
         return QVariant();

      const Node* node = static_cast<Node*>(index.internalPointer());
      if (!node)
         return QVariant();

      return QVariant(QString::fromWCharArray(node->TextPtr));
   }

   QVariant TextTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
   {
      return QVariant();
   }

   QModelIndex TextTreeModel::index(int row, int column, const QModelIndex& parent) const
   {
      if (!Tree)
         return QModelIndex();

      const Node* parentNode = parent.isValid() ? static_cast<Node*>(parent.internalPointer()) : nullptr;
      const std::vector<Node*>& nodes = parentNode ? parentNode->Children : Tree->Roots;
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

      const Node* node = static_cast<Node*>(index.internalPointer());
      if (!node)
         return QModelIndex();

      return createIndex(int(node->IndexInParent), 0, static_cast<void*>(node->Parent));
   }

   int TextTreeModel::rowCount(const QModelIndex& parent) const
   {
      if (!Tree)
         return 0;

      if (!parent.isValid())
         return int(Tree->Roots.size());

      const Node* node = static_cast<Node*>(parent.internalPointer());
      if (!node)
         return 0;

      return int(node->Children.size());
   }
   int TextTreeModel::columnCount(const QModelIndex& parent) const
   {
      if (!Tree)
         return 0;
      return 1;
   }
}
