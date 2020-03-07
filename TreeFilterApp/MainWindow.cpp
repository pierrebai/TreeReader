#include "MainWindow.h"
#include "TextTreeModel.h"
#include "QtUtilities.h"

#include "TreeFilterMaker.h"

#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWinExtras/qwinfunctions.h>

#include "resource.h"

namespace TreeReaderApp
{
   using namespace TreeReader;
   using namespace QtAdditions;
   using namespace std;

   namespace
   {
      namespace L
      {
         inline const wchar_t* t(const wchar_t* text)
         {
            return text;
         }
      }

      static constexpr wchar_t TreeFileTypes[] = L"Text Tree files (*.txt *.log);;Text files (*.txt);;Log files (*.log)";
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
      QToolBar* toolbar = new QToolBar();
         toolbar->setIconSize(QSize(32, 32));

         _loadTreeAction = CreateAction(L::t(L"Load Tree"), IDB_TREE_OPEN, QKeySequence(QKeySequence::StandardKey::Open));
         _loadTreeButton = CreateToolButton(_loadTreeAction);
         toolbar->addWidget(_loadTreeButton);

         _saveTreeAction = CreateAction(L::t(L"Save Tree"), IDB_TREE_SAVE, QKeySequence(QKeySequence::StandardKey::Save));
         _saveTreeButton = CreateToolButton(_saveTreeAction);
         toolbar->addWidget(_saveTreeButton);

         _applyFilterAction = CreateAction(L::t(L"Filter Tree"), IDB_FILTER_APPLY, QKeySequence(QKeySequence::StandardKey::Find));
         _applyFilterButton = CreateToolButton(_applyFilterAction);
         toolbar->addWidget(_applyFilterButton);

         _nameFilterAction = CreateAction(L::t(L"Name Filter"), IDB_FILTER_NAME);
         _nameFilterButton = CreateToolButton(_nameFilterAction);
         toolbar->addWidget(_nameFilterButton);

         toolbar->addSeparator();

         _undoAction = CreateAction(L::t(L"Undo"), IDB_UNDO, QKeySequence(QKeySequence::StandardKey::Undo));
         _undoButton = CreateToolButton(_undoAction);
         _undoAction->setEnabled(false);
         toolbar->addWidget(_undoButton);

         _redoAction = CreateAction(L::t(L"Redo"), IDB_REDO, QKeySequence(QKeySequence::StandardKey::Redo));
         _redoButton = CreateToolButton(_redoAction);
         _redoAction->setEnabled(false);
         toolbar->addWidget(_redoButton);


      auto filtersDock = new QDockWidget(QString::fromWCharArray(L::t(L"Tree Filter")));
         filtersDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
         QWidget* filters_container = new QWidget();
         QHBoxLayout* filters_layout = new QHBoxLayout(filters_container);

         _availableFiltersList = new TreeFilterListWidget;
         _scrollFiltersList = new QWidgetScrollListWidget(_availableFiltersList);
         filters_layout->addWidget(_scrollFiltersList);

         _filterEditor = new FilterEditor(_data.UndoRedo(), filters_container);
         filters_layout->addWidget(_filterEditor);

         filtersDock->setWidget(filters_container);

      _treeView = new QTreeView;
      _treeView->setUniformRowHeights(true);
      _treeView->setHeaderHidden(true);
      _treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

      setCentralWidget(_treeView);
      addToolBar(toolbar);
      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, filtersDock);
      setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 256, 256, 0))));
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Connect the signals of the UI elements.

   void MainWindow::ConnectUI()
   {
      _data.UndoRedo().Changed = [self = this](UndoStack&)
      {
         self->UpdateUndoRedoActions();
      };

      /////////////////////////////////////////////////////////////////////////
      //
      // Undo / redo actions.

      _undoAction->connect(_undoAction, &QAction::triggered, [self=this]()
      {
         self->_data.UndoRedo().Undo();
         self->FillFilterEditorUI();
      });

      _redoAction->connect(_redoAction, &QAction::triggered, [self = this]()
      {
         self->_data.UndoRedo().Redo();
         self->FillFilterEditorUI();
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
      try
      {
         _data.LoadNamedFilters(L"filters.txt");
      }
      catch (const exception &)
      {
         // Ignore.
      }

      _data.ClearUndoStack();
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
         _treeView->setModel(model);
      }
   }

   void MainWindow::FillFilterEditorUI()
   {
      _filterEditor->SetEdited(_data.GetFilter(), L"");
   }

   void MainWindow::FillAvailableFiltersUI()
   {
      for (const auto& filter : _data.GetNamedFilters())
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
            self->_data.SetFilter(named->Filter);
            self->_filterEditor->SetEdited(named->Filter, named->Name, true);
         }
      };

      _availableFiltersList->AddTreeFilter(filter, delCallback, editCallback);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Closing and saving.

   void MainWindow::closeEvent(QCloseEvent* ev)
   {
      if (SaveIfRequired(L::t(L"close the window"), L::t(L"closing the window")))
      {
         QWidget::closeEvent(ev);
         try
         {
            _data.SaveNamedFilters(L"filters.txt");
         }
         catch (const exception&)
         {
            // Ignore.
         }
      }
      else
      {
         ev->ignore();
      }
   }

   bool MainWindow::SaveIfRequired(const wstring& action, const wstring& actioning)
   {
      if (_data.GetFilteredTree() && !_data.IsFilteredTreeSaved())
      {
         YesNoCancel answer = AskYesNoCancel(
            L::t(L"Unsaved Text Tree Warning"),
            wstring(L::t(L"The current filtered tree has not been saved.\nDo you want to save it before ")) + actioning + L::t(L"?"),
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
      if (!SaveIfRequired(L::t(L"load a text tree"), L::t(L"loading a text tree")))
         return;

      filesystem::path path = AskOpen(L::t(L"Load Text Tree"), L::t(TreeFileTypes), this);
      _data.LoadTree(path);

      if (_data.GetCurrentTree() == nullptr)
         return;

      FillTextTreeUI();
   }

   bool MainWindow::SaveFilteredTree()
   {
      if (!_data.GetFilteredTree())
         return true;

      filesystem::path path = AskSave(L::t(L"Save Filtered Text Tree"), L::t(TreeFileTypes), L"",  this);

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

      _data.ApplyFilterToTree();
      FillTextTreeUI();
   }

   void MainWindow::NameFilter()
   {
      auto filter = _filterEditor->GetEdited();
      if (!filter)
         return;

      wstring filterName = _filterEditor->GetEditedName();

      filterName = AskForText(L::t(L"Name a filter"), L::t(L"Filter Name"), filterName.c_str(), this);
      if (filterName.empty())
         return;

      auto namedFilter = _data.NameFilter(filterName, filter->Clone());
      AddNamedFilterToAvailable(namedFilter);
   }

   void MainWindow::UpdateUndoRedoActions()
   {
      _undoAction->setEnabled(_data.UndoRedo().HasUndo());
      _redoAction->setEnabled(_data.UndoRedo().HasRedo());
   }

}
