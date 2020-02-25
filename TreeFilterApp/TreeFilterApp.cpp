#include "TreeReader.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtWidgets/qtreeview.h>
#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlayout.h>

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

      if (!parent.isValid())
         return int(tree->Roots.size());

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

   auto container = new QWidget(mainWindow);
   auto layout = new QGridLayout(container);
   layout->setColumnStretch(0, 80);
   layout->setColumnStretch(1, 20);
   container->setLayout(layout);

   mainWindow->setCentralWidget(container);

   auto treeView = new QTreeView(mainWindow);
   treeView->setUniformRowHeights(true);
   layout->addWidget(treeView, 0, 0, 1, 1);

   auto cmd = new QLineEdit(mainWindow);
   layout->addWidget(cmd, 1, 0, 1, 2);
   
   auto output = new QTextEdit(mainWindow);
   layout->addWidget(output, 0, 1, 1, 1);

   CommandsContext ctx;
   cmd->connect(cmd, &QLineEdit::editingFinished, [&]()
   {
      QString text = cmd->text();
      wstring result = ParseCommands(text.toStdWString(), ctx);

      if (result.size())
        output->setText(QString::fromStdWString(result));

      if (ctx.Filtered)
      {
         TextTreeModel* model = new TextTreeModel;
         model->tree = ctx.Filtered;
         treeView->setModel(model);
      }
      else if (ctx.Tree)
      {
         TextTreeModel* model = new TextTreeModel;
         model->tree = ctx.Tree;
         treeView->setModel(model);
      }
   });

   mainWindow->resize(1000, 800);
   mainWindow->show();
   return app->exec();
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow)
{
   appInstance = hInstance;
   return main(0, 0);
}
