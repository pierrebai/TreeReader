#include "TreeReader.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtWidgets/qtreeview.h>

using namespace std;
using namespace TreeReader;

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

      const size_t nodeIndex = index.internalId();
      if (nodeIndex >= tree->Nodes.size())
         return QVariant();

      return QVariant(QString::fromWCharArray(tree->Nodes[nodeIndex].TextPtr));
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

      const size_t parentIndex = parent.isValid() ? size_t(parent.internalId()) : TextTree::InvalidIndex;
      size_t index = (parentIndex < tree->Nodes.size()) ? tree->Nodes[parentIndex].FirstChildIndex : 0;
      while (row > 0 && index < tree->Nodes.size())
      {
         index = tree->Nodes[index].NextSiblingIndex;
         --row;
      }

      if (row > 0)
         return QModelIndex();

      return createIndex(row, column, index);
   }

   QModelIndex parent(const QModelIndex& index) const override
   {
      // TODO
      if (!tree)
         return QModelIndex();

      if (!index.isValid())
         return QModelIndex();

      const size_t nodeIndex = index.internalId();
      if (nodeIndex >= tree->Nodes.size())
         return QModelIndex();

      const size_t parentIndex = tree->Nodes[nodeIndex].ParentIndex;
      const size_t row = tree->Nodes[nodeIndex].ChildInParent;

      return createIndex(int(row), 0, parentIndex);
   }

   int rowCount(const QModelIndex& parent = QModelIndex()) const override
   {
      if (!tree)
         return 0;

      return int(tree->CountChildren(parent.isValid() ? size_t(parent.internalId()) : TextTree::InvalidIndex));
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
