#include "MainWindow.h"
#include "TextTreeModel.h"
#include "OptionsDialog.h"

#include "QtUtilities.h"

#include "TreeFilterMaker.h"
#include "TreeReaderHelpers.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qtreeview.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>

#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

#include <QtWinExtras/qwinfunctions.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qtimer.h>

#include <fstream>
#include <iomanip>

#include "resource.h"

namespace TreeReaderApp
{
   using namespace TreeReader;
   using namespace QtAdditions;
   using namespace std;

   namespace
   {
      static constexpr char TreeFileTypes[] = "Text Tree files (*.txt *.log);;Text files (*.txt);;Log files (*.log)";

      static filesystem::path GetLocalDataFileName(const QString& filename)
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

      static filesystem::path GetNamedFiltersFileName()
      {
         return GetLocalDataFileName("tree-reader-named-filters.txt");
      }

      static filesystem::path GetOptionsFileName()
      {
         return GetLocalDataFileName("tree-reader-options.txt");
      }

      static filesystem::path GetMainWindowStateFileName()
      {
         return GetLocalDataFileName("main-window-state.txt");
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

      auto toolbar = new QToolBar();
         toolbar->setObjectName("Main Toolbar");
         toolbar->setIconSize(QSize(32, 32));

         _loadTreeAction = CreateAction(tr("Load Tree"), IDB_TREE_OPEN, QKeySequence(QKeySequence::StandardKey::Open));
         _loadTreeButton = CreateToolButton(_loadTreeAction);
         toolbar->addWidget(_loadTreeButton);

         _saveTreeAction = CreateAction(tr("Save Tree"), IDB_TREE_SAVE, QKeySequence(QKeySequence::StandardKey::Save));
         _saveTreeButton = CreateToolButton(_saveTreeAction);
         toolbar->addWidget(_saveTreeButton);

         _applyFilterAction = CreateAction(tr("Filter Tree"), IDB_FILTER_APPLY, QKeySequence(QKeySequence::StandardKey::Find));
         _applyFilterButton = CreateToolButton(_applyFilterAction);
         toolbar->addWidget(_applyFilterButton);

         _nameFilterAction = CreateAction(tr("Name Filter"), IDB_FILTER_NAME);
         _nameFilterButton = CreateToolButton(_nameFilterAction);
         toolbar->addWidget(_nameFilterButton);

         toolbar->addSeparator();

         _undoAction = CreateAction(tr("Undo"), IDB_UNDO, QKeySequence(QKeySequence::StandardKey::Undo));
         _undoButton = CreateToolButton(_undoAction);
         _undoAction->setEnabled(false);
         toolbar->addWidget(_undoButton);

         _redoAction = CreateAction(tr("Redo"), IDB_REDO, QKeySequence(QKeySequence::StandardKey::Redo));
         _redoButton = CreateToolButton(_redoAction);
         _redoAction->setEnabled(false);
         toolbar->addWidget(_redoButton);

         toolbar->addSeparator();

         _optionsAction = CreateAction(tr("Options"), IDB_OPTIONS);
         _optionsButton = CreateToolButton(_optionsAction);
         toolbar->addWidget(_optionsButton);

      auto filtersDock = new QDockWidget(tr("Tree Filter"));
         filtersDock->setObjectName("Tree Filter");
         filtersDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
         auto filtersContainer = new QWidget();
         auto filtersLayout = new QHBoxLayout(filtersContainer);

         _availableFiltersList = new TreeFilterListWidget;
         _scrollFiltersList = new QWidgetScrollListWidget(_availableFiltersList);
         filtersLayout->addWidget(_scrollFiltersList);

         _filterEditor = new FilterEditor(_data.GetNamedFilters(), _data.UndoRedo(), filtersContainer);
         filtersLayout->addWidget(_filterEditor);

         filtersDock->setWidget(filtersContainer);

      auto simpleSearchDock = new QDockWidget(tr("Tree Text Search"));
         simpleSearchDock->setObjectName("Tree Text Search");
         simpleSearchDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
         auto searchContainer = new QWidget();
         auto searchLayout = new QHBoxLayout(searchContainer);

         _simpleSearch = new QLineEdit;
         searchLayout->addWidget(_simpleSearch);

         _editSearchButton = new QPushButton;
         _editSearchButton->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_EDIT)));
         _editSearchButton->setToolTip(tr("Edit Search Filter"));
         _editSearchButton->setFlat(true);
         _editSearchButton->setMaximumSize(QSize(16, 16));

         searchLayout->addWidget(_editSearchButton);

         simpleSearchDock->setWidget(searchContainer);

      auto mainContainer = new QWidget;
         auto mainLayout = new QVBoxLayout(mainContainer);

         _treeView = new QTreeView;
         _treeView->setUniformRowHeights(true);
         _treeView->setHeaderHidden(true);
         _treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

         mainLayout->addWidget(_treeView);

      setCentralWidget(mainContainer);
      addToolBar(toolbar);
      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, filtersDock);
      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, simpleSearchDock);
      setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 256, 256, 0))));
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Connect the signals of the UI elements.

   void MainWindow::ConnectUI()
   {
      _filteringTimer->connect(_filteringTimer, &QTimer::timeout, [self = this]()
      {
         self->verifyAsyncFiltering();
      });

      _data.UndoRedo().Changed = [self = this](UndoStack&)
      {
         self->UpdateUndoRedoActions();
      };

      /////////////////////////////////////////////////////////////////////////
      //
      // Simple text search.

      _editSearchButton->connect(_editSearchButton, &QPushButton::clicked, [self = this]()
      {
         auto filter = ConvertSimpleTextToFilters(self->_simpleSearch->text().toStdWString(), self->_data.GetNamedFilters());
         if (filter)
         {
            self->_filterEditor->SetEdited(filter, L"");
         }
      });

      _simpleSearch->connect(_simpleSearch, &QLineEdit::textChanged, [self = this](const QString& text)
      {
         self->SearchInTree(text);
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Undo / redo actions.

      _undoAction->connect(_undoAction, &QAction::triggered, [self=this]()
      {
         self->_data.UndoRedo().Undo();
      });

      _redoAction->connect(_redoAction, &QAction::triggered, [self = this]()
      {
         self->_data.UndoRedo().Redo();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Load / save actions.

      _loadTreeAction->connect(_loadTreeAction, &QAction::triggered, [self=this]()
      {
         self->LoadTree();
      });

      _saveTreeAction->connect(_saveTreeAction, &QAction::triggered, [self=this]()
      {
         self->SaveFilteredTree();
      });

      _applyFilterAction->connect(_applyFilterAction, &QAction::triggered, [self = this]()
      {
         self->FilterTree();
      });

      _nameFilterAction->connect(_nameFilterAction, &QAction::triggered, [self = this]()
      {
         self->NameFilter();
      });

      _optionsAction->connect(_optionsAction, &QAction::triggered, [self = this]()
      {
         self->OpenOptions();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // The filter list UI call-backs.

      _filterEditor->FilterChanged = [self=this](const TreeFilterPtr& filter)
      {
         self->_data.SetFilter(self->_filterEditor->GetEdited());
      };
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Fill the UI with the intial data.

   void MainWindow::FillUI()
   {
      WithNoExceptions([self = this]() { self->_data.LoadNamedFilters(GetNamedFiltersFileName()); });
      WithNoExceptions([self = this]() { self->_data.LoadOptions(GetOptionsFileName()); });
      WithNoExceptions([self = this]() { self->LoadState(); });

      FillAvailableFiltersUI();
   }

   void MainWindow::FillTextTreeUI()
   {
      shared_ptr<TextTree> newTree;
      if (_data.GetFilteredTree())
      {
         newTree = _data.GetFilteredTree();
      }
      else
      {
         newTree = _data.GetCurrentTree();
      }

      if (!_treeView->model() || !dynamic_cast<TextTreeModel*>(_treeView->model()) || dynamic_cast<TextTreeModel*>(_treeView->model())->Tree != newTree)
      {
         TextTreeModel* model = new TextTreeModel;
         model->Tree = newTree;
         auto oldModel = _treeView->model();
         _treeView->setModel(model);
         delete oldModel;
      }
   }

   void MainWindow::FillFilterEditorUI()
   {
      _filterEditor->SetEdited(_data.GetFilter(), L"");
   }

   void MainWindow::FillAvailableFiltersUI()
   {
      for (const auto& [name, filter] : _data.GetNamedFilters().All())
         AddNamedFilterToAvailable(filter);

      _availableFiltersList->AddTreeFilter(Accept());
      _availableFiltersList->AddTreeFilter(Stop());
      _availableFiltersList->AddTreeFilter(Until(nullptr));
      _availableFiltersList->AddTreeFilter(Contains(L""));
      _availableFiltersList->AddTreeFilter(Regex(L""));
      _availableFiltersList->AddTreeFilter(Not(nullptr));
      _availableFiltersList->AddTreeFilter(Any(vector<TreeFilterPtr>()));
      _availableFiltersList->AddTreeFilter(All(vector<TreeFilterPtr>()));
      _availableFiltersList->AddTreeFilter(Under(nullptr));
      _availableFiltersList->AddTreeFilter(CountSiblings(nullptr, 0));
      _availableFiltersList->AddTreeFilter(CountChildren(nullptr, 0));
      _availableFiltersList->AddTreeFilter(NoChild(nullptr));
      _availableFiltersList->AddTreeFilter(LevelRange(0, 100));
      _availableFiltersList->AddTreeFilter(IfSubTree(nullptr));
      _availableFiltersList->AddTreeFilter(IfSibling(nullptr));
   }

   void MainWindow::AddNamedFilterToAvailable(const TreeFilterPtr& filter)
   {
      auto delCallback = [self = this](TreeFilterListItem* panel)
      {
         if (!panel)
            return;

         if (!panel->Filter)
            return;

         if (self->_data.RemoveNamedFilter(panel->Filter->GetName()))
         {
            self->_availableFiltersList->RemoveItem(panel);
         }
      };

      auto editCallback = [self = this](TreeFilterListItem* panel)
      {
         if (!panel)
            return;

         if (auto named = dynamic_pointer_cast<NamedTreeFilter>(panel->Filter))
         {
            if (named->Filter)
            {
               self->_filterEditor->SetEdited(named->Filter->Clone(), named->Name, true);
            }
         }
      };

      _availableFiltersList->AddTreeFilter(filter, delCallback, editCallback);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Main window state.

   void MainWindow::SaveState()
   {
      ofstream stream(GetMainWindowStateFileName());
      QByteArray state = saveState();
      stream << state.toBase64().toStdString();
   }

   void MainWindow::LoadState()
   {
      ifstream stream(GetMainWindowStateFileName());
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
         WithNoExceptions([self = this]() { self->_data.SaveNamedFilters(GetNamedFiltersFileName()); });
         WithNoExceptions([self = this]() { self->_data.SaveOptions(GetOptionsFileName()); });
         WithNoExceptions([self = this]() { self->SaveState(); });
         WithNoExceptions([self = this]() { self->_data.AbortAsyncFilter(); });

         QWidget::closeEvent(ev);
      }
      else
      {
         ev->ignore();
      }
   }

   bool MainWindow::SaveIfRequired(const QString& action, const QString& actioning)
   {
      if (_data.GetFilteredTree() && !_data.IsFilteredTreeSaved())
      {
         YesNoCancel answer = AskYesNoCancel(
            tr("Unsaved Text Tree Warning"),
            QString(tr("The current filtered tree has not been saved.\nDo you want to save it before ")) + actioning + QString(tr("?")),
            this);
         if (answer == YesNoCancel::Cancel)
            return false;
         else if (answer == YesNoCancel::Yes)
            if (!SaveFilteredTree())
               return false;
      }

      return true;
   }

   void MainWindow::LoadTree()
   {
      if (!SaveIfRequired(tr("load a text tree"), tr("loading a text tree")))
         return;

      filesystem::path path = AskOpen(tr("Load Text Tree"), tr(TreeFileTypes), this);
      _data.LoadTree(path);

      if (_data.GetCurrentTree() == nullptr)
         return;

      FillTextTreeUI();
   }

   bool MainWindow::SaveFilteredTree()
   {
      if (!_data.GetFilteredTree())
         return true;

      filesystem::path path = AskSave(tr("Save Filtered Text Tree"), tr(TreeFileTypes), "",  this);

      _data.SaveFilteredTree(path);

      return true;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree filtering.

   void MainWindow::FilterTree()
   {
      _data.SetFilter(_filterEditor->GetEdited());

      if (_data.GetCurrentTree() == nullptr)
         return;

      _data.ApplyFilterToTreeAsync();
      _filteringTimer->start(10);

   }

   void MainWindow::verifyAsyncFiltering()
   {
      if (_data.IsAsyncFilterReady())
      {
         FillTextTreeUI();
      }
      else
      {
         _filteringTimer->start(10);
      }
   }

   void MainWindow::SearchInTree(const QString& text)
   {
      _data.SearchInTreeAsync(text.toStdWString());
      _filteringTimer->start(10);
   }

   void MainWindow::NameFilter()
   {
      auto filter = _filterEditor->GetEdited();
      if (!filter)
         return;

      wstring filterName = _filterEditor->GetEditedName();

      filterName = AskForText(tr("Name a filter"), tr("Filter Name"), QString::fromStdWString(filterName), this);
      if (filterName.empty())
         return;

      auto namedFilter = _data.NameFilter(filterName, filter->Clone());
      AddNamedFilterToAvailable(namedFilter);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Options.

   void MainWindow::OpenOptions()
   {
      auto dialog = new OptionsDialog(_data.Options, this);
      dialog->exec();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Undo/redo.
   void MainWindow::UpdateUndoRedoActions()
   {
      _undoAction->setEnabled(_data.UndoRedo().HasUndo());
      _redoAction->setEnabled(_data.UndoRedo().HasRedo());
   }

}
