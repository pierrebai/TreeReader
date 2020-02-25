#include "TreeReader.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtWidgets/qtreeview.h>

using namespace std;
using namespace TreeReader;
using Node = TextTree::Node;

static HINSTANCE appInstance;

struct TextTreeModel : QAbstractItemModel
{
   shared_ptr<TextTree> tree;

   TextTreeModel(QObject* parent = 0)
   {
   }

   ~TextTreeModel()
   {
   }

   QVariant data(const QModelIndex& index, int role) const override
   {
      if (!tree)
         return QVariant();

      if (role != Qt::DisplayRole)
         return QVariant();

      if (!index.isValid())
         return QVariant();

      const Node * node = static_cast<Node *>(index.internalPointer());
      if (!node)
         return QVariant();

      return QVariant(QString::fromWCharArray(node->TextPtr));
   }

   QVariant headerData(int section, Qt::Orientation orientation,
      int role = Qt::DisplayRole) const override
   {
      if (!tree)
         return QVariant();

      if (role != Qt::DisplayRole)
         return QVariant();

      if (orientation != Qt::Horizontal)
         return QVariant();

      if (section != 0)
         return QVariant();

      return QVariant(QString::fromWCharArray(L"Text"));
   }

   QModelIndex index(int row, int column,
      const QModelIndex& parent = QModelIndex()) const override
   {
      if (!tree)
         return QModelIndex();

      const Node* parentNode = parent.isValid() ? static_cast<Node*>(parent.internalPointer()) : nullptr;
      const std::vector<Node*>& nodes = parentNode ? parentNode->Children : tree->Roots;
      if (row < 0 || row >= nodes.size())
         return QModelIndex();

      return createIndex(row, column, static_cast<void *>(nodes[row]));
   }

   QModelIndex parent(const QModelIndex& index) const override
   {
      if (!tree)
         return QModelIndex();

      if (!index.isValid())
         return QModelIndex();

      const Node* node = static_cast<Node*>(index.internalPointer());
      if (!node)
         return QModelIndex();

      return createIndex(int(node->IndexInParent), 0, static_cast<void *>(node->Parent));
   }

   int rowCount(const QModelIndex& parent = QModelIndex()) const override
   {
      if (!tree)
         return 0;

      const Node* node = static_cast<Node*>(parent.internalPointer());
      if (!node)
         return 0;

      return int(node->Children.size());
   }
   int columnCount(const QModelIndex& parent = QModelIndex()) const override
   {
      if (!tree)
         return 0;
      return 1;
   }

};

int main(int argc, char** argv)
{
   QScopedPointer<QApplication> app(new QApplication(argc, argv));
   auto mainWindow = new QMainWindow;

   auto treeView = new QTreeView(mainWindow);

   ReadSimpleTextTreeOptions readOptions;
   auto tree = make_shared<TextTree>(ReadSimpleTextTree(filesystem::path(L"TagLogger.log"), readOptions));
   TextTreeModel* model = new TextTreeModel;
   model->tree = tree;
   treeView->setUniformRowHeights(true);
   treeView->setModel(model);

   mainWindow->setCentralWidget(treeView);

   mainWindow->resize(1000, 800);
   mainWindow->show();
   return app->exec();
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow)
{
   appInstance = hInstance;
   return main(0, 0);
}
