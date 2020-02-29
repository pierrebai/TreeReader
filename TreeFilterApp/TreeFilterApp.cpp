#include "TextTreeModel.h"
#include "TreeFilterCommands.h"

#include "MainWindow.h"
#include "resource.h"

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

      MainWindowIcons icons;

      icons.AppIcon = IDI_APP_ICON;

      icons.TextTreeOpen = IDB_TREE_OPEN;
      icons.TextTreeSave = IDB_TREE_SAVE;

      icons.Undo = IDB_UNDO;
      icons.Redo = IDB_REDO;

      icons.FilterAdd = IDB_FILTER_ADD;
      icons.FilterCopy = IDB_FILTER_COPY;
      icons.FilterDelete = IDB_FILTER_DELETE;
      icons.FilterMoveDown = IDB_FILTER_MOVE_DOWN;
      icons.FilterMoveUp = IDB_FILTER_MOVE_UP;

      auto mainWindow = new MainWindow(icons);

      //auto output = new QTextEdit(mainWindow);
      //output->setSizeAdjustPolicy(QTextEdit::SizeAdjustPolicy::AdjustToContents);
      //output->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
      //layout->addWidget(output, 0, 1, 1, 1);

      //CommandsContext ctx;
      //try
      //{
      //   ctx.NamedFilters = ReadNamedFilters(L"filters.txt");
      //}
      //catch (const exception &)
      //{
      //   // Ignore.
      //}

      mainWindow->resize(1000, 800);
      mainWindow->show();

      const int result = app->exec();

      //try
      //{
      //   WriteNamedFilters(L"filters.txt", ctx.NamedFilters);
      //}
      //catch (const exception &)
      //{
      //   // Ignore.
      //}

      return result;
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
