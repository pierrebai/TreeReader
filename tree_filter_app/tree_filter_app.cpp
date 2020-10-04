#include "main_window.h"

#include <QtWidgets/qapplication.h>

#include <QtCore/qlibraryinfo.h>
#include <QtCore/qtranslator.h>

static HINSTANCE app_instance;

namespace dak::tree_reader::app
{
   using namespace std;

   int App(int argc, char** argv)
   {
      QScopedPointer<QApplication> app(new QApplication(argc, argv));

      QTranslator qt_translator;
      qt_translator.load("qt_" + QLocale::system().name(),
         QLibraryInfo::location(QLibraryInfo::TranslationsPath));
      app->installTranslator(&qt_translator);

      QTranslator app_translator;
      app_translator.load("TreeFilterApp_" + QLocale::system().name());
      app->installTranslator(&app_translator);

      auto window = new main_window_t;
      window->resize(1000, 800);
      window->show();

      return app->exec();
   }
}

int main(int argc, char** argv)
{
   return dak::tree_reader::app::App(argc, argv);
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow)
{
   app_instance = hInstance;
   return main(0, 0);
}
