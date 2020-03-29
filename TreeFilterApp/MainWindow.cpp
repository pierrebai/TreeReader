#include "MainWindow.h"
#include "TextTreeModel.h"
#include "TextTreeSubWindow.h"
#include "OptionsDialog.h"

#include "QtUtilities.h"

#include "dak/tree_reader/tree_filter_maker.h"
#include "dak/utility/exceptions.h"
#include "dak/tree_reader/tree_commands.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qmdiarea.h>

#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

#include <QtWinExtras/qwinfunctions.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qtimer.h>

#include <fstream>
#include <iomanip>

#include "resource.h"

namespace dak::tree_reader::app
{
   using namespace dak::tree_reader;
   using namespace dak::utility;
   using namespace Qtadditions;
   using namespace std;

   namespace
   {
      static filesystem::path getLocalDataFileName(const QString& filename)
      {
         auto path = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, filename, QStandardPaths::LocateFile);
         if (!path.isEmpty())
            return path.toStdWString();

         auto location = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
         if (location.size() <= 0)
            return filename.toStdWString();

         filesystem::path dir(location.toStdWString());
         error_code error;
         filesystem::create_directories(dir, error);

         return dir / filesystem::path(filename.toStdWString());
      }

      static filesystem::path get_FiltersFileName()
      {
         return getLocalDataFileName("tree-reader-named-filters.txt");
      }

      static filesystem::path getOptionsFileName()
      {
         return getLocalDataFileName("tree-reader-options.txt");
      }

      static filesystem::path getMainWindowStateFileName()
      {
         return getLocalDataFileName("main-window-state.txt");
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the main window.

   MainWindow::MainWindow()
   {
      BuildUI();
      FillUI();
      ConnectUI();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the UI elements.

   void MainWindow::BuildUI()
   {
      setCorner(Qt::Corner::TopLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
      setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
      setCorner(Qt::Corner::TopRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);
      setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);

      _filteringTimer = new QTimer(this);
      _filteringTimer->setSingleShot(true);

      BuildToolBarUI();
      BuildFiltersUI();
      BuildSimpleSearchUI();
      BuildTabbedUI();

      setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 256, 256, 0))));
   }

   void MainWindow::BuildToolBarUI()
   {
      auto toolbar = new QToolBar();
      toolbar->setObjectName("Main Toolbar");
      toolbar->setIconSize(QSize(32, 32));

      _loadTreeAction = CreateAction(tr("Load Tree"), IDB_TREE_OPEN, QKeySequence(QKeySequence::StandardKey::Open));
      _loadTreeButton = CreateToolButton(_loadTreeAction);
      toolbar->addWidget(_loadTreeButton);

      _saveTreeAction = CreateAction(tr("Save Tree"), IDB_TREE_SAVE, QKeySequence(QKeySequence::StandardKey::Save));
      _saveTreeButton = CreateToolButton(_saveTreeAction);
      toolbar->addWidget(_saveTreeButton);

      _applyFilterAction = CreateAction(tr("filter Tree"), IDB_FILTER_APPLY, QKeySequence(QKeySequence::StandardKey::Find));
      _applyFilterButton = CreateToolButton(_applyFilterAction);
      toolbar->addWidget(_applyFilterButton);

      _nameFilterAction = CreateAction(tr("Name filter"), IDB_FILTER_NAME);
      _nameFilterButton = CreateToolButton(_nameFilterAction);
      toolbar->addWidget(_nameFilterButton);

      _pushFilterAction = CreateAction(tr("Use Filtered"), IDB_FILTER_PUSH);
      _pushFilterButton = CreateToolButton(_pushFilterAction);
      toolbar->addWidget(_pushFilterButton);

      toolbar->addSeparator();

      _undoAction = CreateAction(tr("undo"), IDB_UNDO, QKeySequence(QKeySequence::StandardKey::Undo));
      _undoButton = CreateToolButton(_undoAction);
      _undoAction->setEnabled(false);
      toolbar->addWidget(_undoButton);

      _redoAction = CreateAction(tr("redo"), IDB_REDO, QKeySequence(QKeySequence::StandardKey::Redo));
      _redoButton = CreateToolButton(_redoAction);
      _redoAction->setEnabled(false);
      toolbar->addWidget(_redoButton);

      toolbar->addSeparator();

      _optionsAction = CreateAction(tr("options"), IDB_OPTIONS);
      _optionsButton = CreateToolButton(_optionsAction);
      toolbar->addWidget(_optionsButton);

      addToolBar(toolbar);
   }

   void MainWindow::BuildFiltersUI()
   {
      auto filtersDock = new QDockWidget(tr("Tree filter"));
      filtersDock->setObjectName("Tree filter");
      filtersDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      auto filtersContainer = new QWidget();
      auto filtersLayout = new QHBoxLayout(filtersContainer);

      _availableFiltersList = new TreeFilterListWidget;
      _scrollFiltersList = new QWidgetScrollListWidget(_availableFiltersList);
      filtersLayout->addWidget(_scrollFiltersList);

      _filterEditor = new FilterEditor(_data.get_Filters(), _data.undo_redo(), filtersContainer);
      filtersLayout->addWidget(_filterEditor);

      filtersDock->setWidget(filtersContainer);

      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, filtersDock);
   }

   void MainWindow::BuildSimpleSearchUI()
   {
      auto simpleSearchDock = new QDockWidget(tr("Tree Text Search"));
      simpleSearchDock->setObjectName("Tree Text Search");
      simpleSearchDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      auto searchContainer = new QWidget();
      auto searchLayout = new QHBoxLayout(searchContainer);

      _simpleSearch = new QLineEdit;
      searchLayout->addWidget(_simpleSearch);

      _editSearchButton = new QPushButton;
      _editSearchButton->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_EDIT)));
      _editSearchButton->setToolTip(tr("Edit Search filter"));
      _editSearchButton->setFlat(true);
      _editSearchButton->setMaximumSize(QSize(16, 16));

      searchLayout->addWidget(_editSearchButton);

      simpleSearchDock->setWidget(searchContainer);

      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, simpleSearchDock);
   }

   void MainWindow::BuildTabbedUI()
   {
      auto mainContainer = new QWidget;
      auto mainLayout = new QVBoxLayout(mainContainer);

      _tabs = new QMdiArea;
      _tabs->setViewMode(QMdiArea::ViewMode::TabbedView);
      _tabs->setDocumentMode(true);
      _tabs->setTabsClosable(true);

      mainLayout->addWidget(_tabs);

      setCentralWidget(mainContainer);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Connect the signals of the UI elements.

   void MainWindow::ConnectUI()
   {
      /////////////////////////////////////////////////////////////////////////
      //
      // Asynchronous filtering.

      _filteringTimer->connect(_filteringTimer, &QTimer::timeout, [self = this]()
      {
         self->VerifyAsyncFiltering();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Simple text search.

      _editSearchButton->connect(_editSearchButton, &QPushButton::clicked, [self = this]()
      {
         auto filter = convert_simple_text_to_filter(self->_simpleSearch->text().toStdWString(), self->_data.get_Filters());
         if (filter)
         {
            self->_filterEditor->SetEdited(filter, L"");
         }
      });

      _simpleSearch->connect(_simpleSearch, &QLineEdit::textChanged, [self = this](const QString& text)
      {
         self->search_in_tree(text);
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // undo / redo actions.

      _data.undo_redo().changed = [self = this](undo_stack&)
      {
         self->Updateundo_redoActions();
      };

      _undoAction->connect(_undoAction, &QAction::triggered, [self=this]()
      {
         self->_data.undo_redo().undo();
      });

      _redoAction->connect(_redoAction, &QAction::triggered, [self = this]()
      {
         self->_data.undo_redo().redo();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Load / save actions.

      _loadTreeAction->connect(_loadTreeAction, &QAction::triggered, [self=this]()
      {
         self->load_tree();
      });

      _saveTreeAction->connect(_saveTreeAction, &QAction::triggered, [self=this]()
      {
         self->save_filtered_tree(self->getCurrentSubWindow());
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Filtering.

      _applyFilterAction->connect(_applyFilterAction, &QAction::triggered, [self = this]()
      {
         self->filter_tree();
      });

      _nameFilterAction->connect(_nameFilterAction, &QAction::triggered, [self = this]()
      {
         self->name_filter();
      });

      _pushFilterAction->connect(_pushFilterAction, &QAction::triggered, [self = this]()
      {
         self->PushFilter();
      });

      _optionsAction->connect(_optionsAction, &QAction::triggered, [self = this]()
      {
         self->OpenOptions();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // The filter list UI call-backs.

      _filterEditor->FilterChanged = [self=this](const tree_filter_ptr& filter)
      {
         auto window = self->getCurrentSubWindow();
         if (!window)
            return;

         window->Tree->set_filter(self->_filterEditor->getEdited());
      };

      /////////////////////////////////////////////////////////////////////////
      //
      // Tabs.

      _tabs->connect(_tabs, &QMdiArea::subWindowActivated, [self = this](QMdiSubWindow* sub)
      {
         self->UpdateActiveTab();
      });
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Fill the UI with the intial data.

   void MainWindow::FillUI()
   {
      with_no_exceptions([self = this]() { self->_data.load_named_filters(get_FiltersFileName()); });
      with_no_exceptions([self = this]() { self->_data.load_options(getOptionsFileName()); });
      with_no_exceptions([self = this]() { self->LoadState(); });

      UpdateCreateTabAction();
      Updateundo_redoActions();

      FillAvailableFiltersUI();
   }

   void MainWindow::FillFilterEditorUI()
   {
      auto window = getCurrentSubWindow();
      if (!window)
         return;

      _filterEditor->SetEdited(window->Tree->get_filter(), window->Tree->get_filter_name());
   }

   void MainWindow::FillAvailableFiltersUI()
   {
      for (const auto& [name, filter] : _data.get_Filters().all())
         addNamedFilterToAvailable(filter);

      _availableFiltersList->addTreeFilter(accept());
      _availableFiltersList->addTreeFilter(stop());
      _availableFiltersList->addTreeFilter(stop_when_kept(nullptr));
      _availableFiltersList->addTreeFilter(until(nullptr));
      _availableFiltersList->addTreeFilter(contains(L""));
      _availableFiltersList->addTreeFilter(regex(L""));
      _availableFiltersList->addTreeFilter(unique());
      _availableFiltersList->addTreeFilter(not(nullptr));
      _availableFiltersList->addTreeFilter(Any(vector<tree_filter_ptr>()));
      _availableFiltersList->addTreeFilter(all(vector<tree_filter_ptr>()));
      _availableFiltersList->addTreeFilter(under(nullptr));
      _availableFiltersList->addTreeFilter(no_child(nullptr));
      _availableFiltersList->addTreeFilter(level_range(0, 100));
      _availableFiltersList->addTreeFilter(if_subtree(nullptr));
      _availableFiltersList->addTreeFilter(if_sibling(nullptr));
   }

   void MainWindow::addNamedFilterToAvailable(const tree_filter_ptr& filter)
   {
      auto delCallback = [self = this](TreeFilterListItem* panel)
      {
         if (!panel)
            return;

         if (!panel->filter)
            return;

         if (self->_data.remove_Filter(panel->filter->get_name()))
         {
            self->_availableFiltersList->removeItem(panel);
         }
      };

      auto editCallback = [self = this](TreeFilterListItem* panel)
      {
         if (!panel)
            return;

         if (auto named = dynamic_pointer_cast<named_tree_filter>(panel->filter))
         {
            if (named->filter)
            {
               self->_filterEditor->SetEdited(named->filter->clone(), named->Name, true);
            }
         }
      };

      _availableFiltersList->addTreeFilter(filter, delCallback, editCallback);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Main window state.

   void MainWindow::SaveState()
   {
      ofstream stream(getMainWindowStateFileName());
      QByteArray state = saveState();
      stream << state.toBase64().toStdString();
   }

   void MainWindow::LoadState()
   {
      ifstream stream(getMainWindowStateFileName());
      string text;
      stream >> text;
      QByteArray state = QByteArray::fromBase64(QByteArray::fromStdString(text));
      restoreState(state, 0);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Closing and saving.

   void MainWindow::closeEvent(QCloseEvent* ev)
   {
      if (SaveIfRequired(tr("close the window"), tr("closing the window")))
      {
         with_no_exceptions([self = this]() { self->_data.save_named_filters(get_FiltersFileName()); });
         with_no_exceptions([self = this]() { self->_data.save_options(getOptionsFileName()); });
         with_no_exceptions([self = this]() { self->SaveState(); });
         with_no_exceptions([self = this]() { self->abort_async_filtering(); });

         QWidget::closeEvent(ev);
      }
      else
      {
         ev->ignore();
      }
   }

   bool MainWindow::SaveIfRequired(const QString& action, const QString& actioning)
   {
      for (auto window : getallSubWindows())
         if (!window->SaveIfRequired(action, actioning))
            return false;

      return true;
   }

   void MainWindow::load_tree()
   {
      filesystem::path path = AskOpen(tr("Load Text Tree"), tr(tree_commands::tree_file_types), this);
      auto newTree = _data.load_tree(path);
      addTextTreeTab(newTree);
      search_in_tree();
   }

   bool MainWindow::save_filtered_tree(TextTreeSubWindow* window)
   {
      if (!window)
         return true;

      return window->save_filtered_tree(_data.options);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tab management.

   void MainWindow::addTextTreeTab(const tree_commands_ptr& newTree)
   {
      if (!newTree)
         return;

      auto subWindow = new TextTreeSubWindow(newTree, _data.options);
      _tabs->addSubWindow(subWindow);
      subWindow->showMaximized();

      FillFilterEditorUI();
      search_in_tree();

      UpdateCreateTabAction();
   }

   void MainWindow::UpdateActiveTab()
   {
      auto window = getCurrentSubWindow();
      if (!window)
         return;

      FillFilterEditorUI();
      search_in_tree();
   }

   void MainWindow::UpdateTextTreeTab()
   {
      auto window = getCurrentSubWindow();
      if (!window)
         return;

      text_tree_ptr newTree;
      if (window->Tree->get_filtered_tree())
      {
         newTree = window->Tree->get_filtered_tree();
      }
      else
      {
         newTree = window->Tree->get_original_tree();
      }

      window->UpdateShownModel(newTree);

      UpdateCreateTabAction();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current tab.

   TextTreeSubWindow* MainWindow::getCurrentSubWindow()
   {
      return dynamic_cast<TextTreeSubWindow*>(_tabs->currentSubWindow());
   }

   vector<TextTreeSubWindow*> MainWindow::getallSubWindows()
   {
      vector<TextTreeSubWindow*> subs;

      for (auto window : _tabs->subWindowList())
         if (auto treeWindow = dynamic_cast<TextTreeSubWindow*>(window))
            subs.push_back(treeWindow);

      return subs;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree filtering.

   void MainWindow::filter_tree()
   {
      auto window = getCurrentSubWindow();
      if (!window)
         return;

      window->Tree->set_filter(_filterEditor->getEdited());

      if (window->Tree->get_original_tree() == nullptr)
         return;

      window->Tree->apply_filter_to_tree_async();
      _filteringTimer->start(10);
   }

   void MainWindow::VerifyAsyncFiltering()
   {
      auto window = getCurrentSubWindow();
      if (!window)
         return;

      if (window->Tree->is_async_filter_ready())
      {
         UpdateTextTreeTab();
      }
      else
      {
         _filteringTimer->start(10);
      }
   }

   void MainWindow::abort_async_filtering()
   {
      for (auto window : getallSubWindows())
         window->Tree->abort_async_filter();
   }

   void MainWindow::search_in_tree()
   {
      search_in_tree(_simpleSearch->text());
   }

   void MainWindow::search_in_tree(const QString& text)
   {
      auto window = getCurrentSubWindow();
      if (!window)
         return;

      window->Tree->search_in_tree_async(text.toStdWString());
      _filteringTimer->start(10);
   }

   void MainWindow::PushFilter()
   {
      auto window = getCurrentSubWindow();
      if (!window)
         return;

      auto newTree = _data.create_tree_from_filtered(window->Tree);
      addTextTreeTab(newTree);
   }

   void MainWindow::UpdateCreateTabAction()
   {
      auto window = getCurrentSubWindow();
      if (!window)
         return;

      _pushFilterAction->setEnabled(window->Tree->can_create_tree_from_filtered());
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // filter naming.

   void MainWindow::name_filter()
   {
      auto filter = _filterEditor->getEdited();
      if (!filter)
         return;

      wstring filterName = _filterEditor->getEditedName();

      filterName = AskForText(tr("Name a filter"), tr("filter Name"), QString::fromStdWString(filterName), this);
      if (filterName.empty())
         return;

      auto namedFilter = _data.name_filter(filterName, filter->clone());
      addNamedFilterToAvailable(namedFilter);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // options.

   void MainWindow::OpenOptions()
   {
      auto dialog = new OptionsDialog(_data.options, this);
      dialog->exec();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // undo/redo.
   void MainWindow::Updateundo_redoActions()
   {
      _undoAction->setEnabled(_data.undo_redo().has_undo());
      _redoAction->setEnabled(_data.undo_redo().has_redo());
   }

}
