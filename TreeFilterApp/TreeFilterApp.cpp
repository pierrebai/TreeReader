#include "TextTreeModel.h"
#include "TreeFilterCommands.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qtreeview.h>
#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlayout.h>

static HINSTANCE appInstance;

namespace TreeReaderApp
{
   using namespace std;
   using namespace TreeReader;
   using Node = TextTree::Node;

   int App(int argc, char** argv)
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
      treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
      layout->addWidget(treeView, 0, 0, 1, 1);

      auto cmd = new QLineEdit(mainWindow);
      cmd->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
      layout->addWidget(cmd, 1, 0, 1, 2);

      auto output = new QTextEdit(mainWindow);
      output->setSizeAdjustPolicy(QTextEdit::SizeAdjustPolicy::AdjustToContents);
      output->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
      layout->addWidget(output, 0, 1, 1, 1);

      CommandsContext ctx;
      cmd->connect(cmd, &QLineEdit::editingFinished, [&]()
      {
         QString text = cmd->text();
         wstring result = ParseCommands(text.toStdWString(), ctx);

         if (result.size())
            output->setText(QString::fromStdWString(result));

         shared_ptr<TextTree> newTree;
         if (ctx.Filtered)
         {
            newTree = ctx.Filtered;
         }
         else if (ctx.Trees.size() > 0)
         {
            newTree = ctx.Trees.back();
         }
         if (!treeView->model() || !dynamic_cast<TextTreeModel*>(treeView->model()) || dynamic_cast<TextTreeModel*>(treeView->model())->Tree != newTree)
         {
            TextTreeModel* model = new TextTreeModel;
            model->Tree = newTree;
            treeView->setModel(model);
         }
      });

      mainWindow->resize(1000, 800);
      mainWindow->show();
      return app->exec();
   }
}

int main(int argc, char** argv)
{
   return TreeReaderApp::App(argc, argv);
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow)
{
   appInstance = hInstance;
   return main(0, 0);
}
