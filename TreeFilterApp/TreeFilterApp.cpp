#include "MainWindow.h"

#include <QtWidgets/qapplication.h>

#include <QtCore/qlibraryinfo.h>
#include <QtCore/qtranslator.h>

static HINSTANCE appInstance;

namespace TreeReaderApp
{
   using namespace std;

   int App(int argc, char** argv)
   {
      QScopedPointer<QApplication> app(new QApplication(argc, argv));

      QTranslator qtTranslator;
      qtTranslator.load("qt_" + QLocale::system().name(),
         QLibraryInfo::location(QLibraryInfo::TranslationsPath));
      app->installTranslator(&qtTranslator);

      QTranslator appTranslator;
      appTranslator.load("TreeFilterApp_" + QLocale::system().name());
      app->installTranslator(&appTranslator);

      auto mainWindow = new MainWindow;
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
