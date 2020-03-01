#include "MainWindow.h"
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

namespace TreeReaderApp
{
   using namespace TreeReader;
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
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the main window.

   MainWindow::MainWindow(const MainWindowIcons& icons)
   {
      BuildUI(icons);
      FillUI();
      ConnectUI(icons);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the UI elements.

   void MainWindow::BuildUI(const MainWindowIcons& icons)
   {
      QToolBar* toolbar = new QToolBar();
         toolbar->setIconSize(QSize(32, 32));

         _loadTreeAction = CreateAction(L::t(L"Load Tree"), icons.TextTreeOpen, QKeySequence(QKeySequence::StandardKey::Open));
         _loadTreeButton = CreateToolButton(_loadTreeAction);
         toolbar->addWidget(_loadTreeButton);

         _saveTreeAction = CreateAction(L::t(L"Save Tree"), icons.TextTreeSave, QKeySequence(QKeySequence::StandardKey::Save));
         _saveTreeButton = CreateToolButton(_saveTreeAction);
         toolbar->addWidget(_saveTreeButton);

         toolbar->addSeparator();

         _undoAction = CreateAction(L::t(L"Undo"), icons.Undo, QKeySequence(QKeySequence::StandardKey::Undo));
         _undoButton = CreateToolButton(_undoAction);
         _undoAction->setEnabled(false);
         toolbar->addWidget(_undoButton);

         _redoAction = CreateAction(L::t(L"Redo"), icons.Redo, QKeySequence(QKeySequence::StandardKey::Redo));
         _redoButton = CreateToolButton(_redoAction);
         _redoAction->setEnabled(false);
         toolbar->addWidget(_redoButton);

      auto filtersDock = new QDockWidget(QString::fromWCharArray(L::t(L"Tree Filter")));
         filtersDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
         QWidget* filters_container = new QWidget();
         QHBoxLayout* filters_layout = new QHBoxLayout(filters_container);

         _availableFiltersList = new QListView;
         _availableFiltersList->setIconSize(QSize(64, 32));
         _availableFiltersList->setDragEnabled(true);
         _availableFiltersList->setDragDropMode(QListView::DragOnly);
         filters_layout->addWidget(_availableFiltersList);

         _filterEditor = new FilterEditor(filters_container, icons.FilterCopy, icons.FilterAdd, icons.FilterDelete, icons.FilterMoveUp, icons.FilterMoveDown);
         filters_layout->addWidget(_filterEditor);

         filtersDock->setWidget(filters_container);

      _treeView = new QTreeView;
      _treeView->setUniformRowHeights(true);
      _treeView->setHeaderHidden(true);
      _treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

      auto cmdDock = new QDockWidget(QString::fromWCharArray(L::t(L"Commands")));
         cmdDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
         QWidget* cmd_container = new QWidget();
         QVBoxLayout* cmd_layout = new QVBoxLayout(cmd_container);

         _cmdLine = new QLineEdit(cmd_container);
         cmd_layout->addWidget(_cmdLine);

         cmdDock->setWidget(cmd_container);

      setCentralWidget(_treeView);
      addToolBar(toolbar);
      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, filtersDock);
      addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, cmdDock);
      setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(icons.AppIcon), IMAGE_ICON, 256, 256, 0))));
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Connect the signals of the UI elements.

   void MainWindow::ConnectUI(const MainWindowIcons& icons)
   {
      /////////////////////////////////////////////////////////////////////////
      //
      // Undo / redo actions.

      _undoAction->connect(_undoAction, &QAction::triggered, [&]()
      {
         _undoStack.Undo();
         UpdateUndoRedoActions();
      });

      _redoAction->connect(_redoAction, &QAction::triggered, [&]()
      {
         _undoStack.Redo();
         UpdateUndoRedoActions();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Load / save actions.

      _loadTreeAction->connect(_loadTreeAction, &QAction::triggered, [self=this]()
      {
         if (!self->SaveIfRequired(L::t(L"load a text tree"), L::t(L"loading a text tree")))
            return;

         filesystem::path path = AskOpen(L::t(L"Load Text Tree"), L::t(L"txt"), self);
         self->_data.LoadTree(path);
         self->_data.ApplyFilterToTree();

         if (self->_data.Trees.size() == 0)
            return;

         self->FillTextTreeUI();
      });

      _saveTreeAction->connect(_saveTreeAction, &QAction::triggered, [self=this]()
      {
         self->SaveFilteredTree();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // The filter list UI call-backs.

      _filterEditor->FiltersChanged = [self=this](const FilterEditor::Filters& filters)
      {
         self->CommitToUndo();
         self->_data.Filter = self->_filterEditor->GetEdited();
      };

      _filterEditor->NewFilterRequested = [self=this]()
      {
         self->RequestNewFilter();
      };

      /////////////////////////////////////////////////////////////////////////
      //
      // Command line-edit.

      _cmdLine->connect(_cmdLine, &QLineEdit::editingFinished, [self=this]()
      {
         QString text = self->_cmdLine->text();
         wstring result = ParseCommands(text.toStdWString(), self->_data);

         self->FillTextTreeUI();
         self->FillFilterEditorUI();
      });
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Fill the UI with the intial data.

   void MainWindow::FillUI()
   {
      try
      {
         *_data.KnownFilters = ReadNamedFilters(L"filters.txt");
      }
      catch (const exception &)
      {
         // Ignore.
      }

      ClearUndoStack();
      FillAvailableFiltersUI();
   }

   void MainWindow::FillTextTreeUI()
   {
      shared_ptr<TextTree> newTree;
      if (_data.Filtered)
      {
         newTree = _data.Filtered;
      }
      else if (_data.Trees.size() > 0)
      {
         newTree = _data.Trees.back();
      }

      if (!_treeView->model() || !dynamic_cast<TextTreeModel*>(_treeView->model()) || dynamic_cast<TextTreeModel*>(_treeView->model())->Tree != newTree)
      {
         TextTreeModel* model = new TextTreeModel;
         model->Tree = newTree;
         _treeView->setModel(model);
      }

      CommitToUndo();
   }

   void MainWindow::FillFilterEditorUI()
   {
      _filterEditor->SetEdited(_data.Filter);
   }

   void MainWindow::FillAvailableFiltersUI()
   {
      _knownFiltersModel = new KnownFiltersModel;

      _knownFiltersModel->KnownFilters = _data.KnownFilters;
      _knownFiltersModel->BaseFilters.push_back(Accept());
      _knownFiltersModel->BaseFilters.push_back(Stop());
      _knownFiltersModel->BaseFilters.push_back(Until(nullptr));
      _knownFiltersModel->BaseFilters.push_back(Contains(L""));
      _knownFiltersModel->BaseFilters.push_back(Regex(L""));
      _knownFiltersModel->BaseFilters.push_back(Not(nullptr));
      _knownFiltersModel->BaseFilters.push_back(Any(vector<TreeFilterPtr>()));
      _knownFiltersModel->BaseFilters.push_back(All(vector<TreeFilterPtr>()));
      _knownFiltersModel->BaseFilters.push_back(Under(nullptr));
      _knownFiltersModel->BaseFilters.push_back(CountSiblings(nullptr, 0));
      _knownFiltersModel->BaseFilters.push_back(CountChildren(nullptr, 0));
      _knownFiltersModel->BaseFilters.push_back(NoChild(nullptr));
      _knownFiltersModel->BaseFilters.push_back(LevelRange(0, 100));
      _knownFiltersModel->BaseFilters.push_back(IfSubTree(nullptr));
      _knownFiltersModel->BaseFilters.push_back(IfSibling(nullptr));

      _availableFiltersList->setModel(_knownFiltersModel);
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
            WriteNamedFilters(L"filters.txt", *_data.KnownFilters);
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
      if (_undoStack.HasUndo() && _data.Trees.size() > 0 && _data.Filter)
      {
         YesNoCancel answer = AskYesNoCancel(
            L::t(L"Unsaved Text Tree Warning"),
            wstring(L::t(L"The current tree has not been saved.\nDo you want to save it before ")) + actioning + L::t(L"?"),
            this);
         if (answer == YesNoCancel::Cancel)
            return false;
         else if (answer == YesNoCancel::Yes)
            if (!SaveFilteredTree())
               return false;
      }

      return true;
   }

   bool MainWindow::SaveFilteredTree()
   {
      if (_data.Trees.size() == 0)
         return true;

      filesystem::path path = AskSave(L::t(L"Save Text Tree"), L::t(L"txt"), this);
      _data.SaveTree(path);

      return true;
   }


   /////////////////////////////////////////////////////////////////////////
   //
   // Undo / redo tool-bar buttons.

   void MainWindow::DeadedFilters(any& data)
   {
      data = ConvertFiltersToText(_data.Filter);
   }

   void MainWindow::AwakenFilters(const any& data)
   {
      _data.Filter = ConvertTextToFilters(any_cast<wstring>(data), *_data.KnownFilters);;

      FillFilterEditorUI();
   }

   void MainWindow::AwakenToEmptyFilters()
   {
      _data.Filter = nullptr;

      FillFilterEditorUI();
   }

   void MainWindow::ClearUndoStack()
   {
      _undoStack.Clear();
      // Note: allow undoing back to an empty filter list. To enable this, there must be an empty initial commit.
      _undoStack.Commit({ 0, nullptr, [self = this](const any&) { self->AwakenToEmptyFilters(); } });
   }

   void MainWindow::CommitToUndo()
   {
      _undoStack.Commit(
      {
         ConvertFiltersToText(_data.Filter),
         [self=this](any& data) { self->DeadedFilters(data); },
         [self=this](const any& data) { self->AwakenFilters(data); }
      });
      UpdateUndoRedoActions();
   }

   void MainWindow::UpdateUndoRedoActions()
   {
      _undoAction->setEnabled(_undoStack.HasUndo());
      _redoAction->setEnabled(_undoStack.HasRedo());
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Layer manipulations.

   void MainWindow::RequestNewFilter()
   {
      // TODO insert a new filter in filter tree...
      //_data.Filter = newFilter;
      _data.ApplyFilterToTree();
      FillFilterEditorUI();
      FillTextTreeUI();
      CommitToUndo();
   }

}
