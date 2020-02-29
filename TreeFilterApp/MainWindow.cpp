#include "MainWindow.h"
#include "TextTreeModel.h"
#include "QtUtilities.h"

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

   MainWindow::MainWindow(const MainWindowIcons& icons)
   {
      BuildUI(icons);
      FillUI();
      ConnectUI(icons);
   }

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

      _layersDock = new QDockWidget(QString::fromWCharArray(L::t(L"Layers")));
         _layersDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
         QWidget* filters_container = new QWidget();
         QVBoxLayout* filters_layout = new QVBoxLayout(filters_container);

         _filtersList = new FiltersEditor(filters_container, icons.FilterCopy, icons.FilterAdd, icons.FilterDelete, icons.FilterMoveUp, icons.FilterMoveDown);
         filters_layout->addWidget(_filtersList);

         _layersDock->setWidget(filters_container);

      _treeView = new QTreeView;
      _treeView->setUniformRowHeights(true);
      _treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

      _cmdDock = new QDockWidget(QString::fromWCharArray(L::t(L"Commands")));
         _cmdDock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
         QWidget* cmd_container = new QWidget();
         QVBoxLayout* cmd_layout = new QVBoxLayout(cmd_container);

         _cmdLine = new QLineEdit(cmd_container);
         cmd_layout->addWidget(_cmdLine);

         _cmdDock->setWidget(cmd_container);

      setCentralWidget(_treeView);
      addToolBar(toolbar);
      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, _layersDock);
      addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, _cmdDock);
      setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(icons.AppIcon), IMAGE_ICON, 256, 256, 0))));
   }

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

         if (self->_data.Trees.size() == 0)
            return;

         self->UpdateTree();
         self->FillFiltersEditor();

         self->ClearUndoStack();
      });

      _saveTreeAction->connect(_saveTreeAction, &QAction::triggered, [self=this]()
      {
         self->SaveFilteredTree();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // The filter list UI call-backs.

      _filtersList->FiltersChanged = [self=this](const FiltersEditor::Filters& filters)
      {
         self->CommitToUndo();
         self->_data.Filter = self->_filtersList->GetEdited();
      };

      _filtersList->SelectionChanged = [self=this](const FiltersEditor::Filters& filters)
      {
         // Nothing...
      };

      _filtersList->NewFilterRequested = [self=this]()
      {
         self->AddFilter(TreeReader::Contains(L"TODO"));
      };

      /////////////////////////////////////////////////////////////////////////
      //
      // Command line-edit.

      _cmdLine->connect(_cmdLine, &QLineEdit::editingFinished, [self=this]()
      {
         QString text = self->_cmdLine->text();
         wstring result = ParseCommands(text.toStdWString(), self->_data);

         self->UpdateTree();
         self->FillFiltersEditor();
      });
   }

   // Fill the UI with the intial data.
   void MainWindow::FillUI()
   {
      // TODO: fill tree view
      try
      {
         _data.KnownFilters = ReadNamedFilters(L"filters.txt");
      }
      catch (const exception &)
      {
         // Ignore.
      }
   }

   void MainWindow::closeEvent(QCloseEvent* ev)
   {
      if (SaveIfRequired(L::t(L"close the window"), L::t(L"closing the window")))
      {
         QWidget::closeEvent(ev);
         try
         {
            WriteNamedFilters(L"filters.txt", _data.KnownFilters);
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
      if (_undoStack.HasUndo())
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
      filesystem::path path;
      // TODO SAVE TREE
      //return ask_save_filtered_mosaic(get_avail_filters(), path, this);
      return false;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // The filters UI call-backs.

   vector<TreeFilterPtr> MainWindow::GetSelectedFilters()
   {
      return _filtersList->GetSelectedFilters();
   }

   void MainWindow::FillFiltersEditor()
   {
      _filtersList->SetEdited(_data.Filter);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Undo / redo tool-bar buttons.

   void MainWindow::DeadedFilters(any& data)
   {
      // Nothing...
   }

   void MainWindow::UpdateUndoRedoActions()
   {
      _undoAction->setEnabled(_undoStack.HasUndo());
      _redoAction->setEnabled(_undoStack.HasRedo());
   }

   void MainWindow::AwakenFilters(const any& data)
   {
      TreeFilterPtr filter = CloneFilters(any_cast<const TreeFilterPtr&>(data));

      _data.Filter = filter;

      FillFiltersEditor();
   }

   void MainWindow::AwakenToEmptyFilters()
   {
      _data.Filter = nullptr;

      FillFiltersEditor();
   }

   void MainWindow::ClearUndoStack()
   {
      _undoStack.Clear();
   }

   void MainWindow::CommitToUndo()
   {
      _undoStack.Commit(
      {
         CloneFilters(_data.Filter),
         [self=this](any& data) { self->DeadedFilters(data); },
         [self=this](const any& data) { self->AwakenFilters(data); }
      });
      UpdateUndoRedoActions();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Layer manipulations.

   TreeFilterPtr MainWindow::CloneFilters(const TreeFilterPtr& filter)
   {
      // TODO CLONE FILTER
      return filter;
   }

   void MainWindow::AddFilter(const TreeFilterPtr& newFilter)
   {
      const bool was_empty = (_data.Filter == nullptr);
      // TODO insert in filter tree... _data.Filter = newFilter;
      _data.Filter = newFilter;
      _data.ApplyFilterToTree();
      FillFiltersEditor();
      UpdateTree();

      if (was_empty)
      {
         ClearUndoStack();
         // Note: when adding filters, allow undoing back to an empty filter list.
         _undoStack.Commit({ 0, nullptr, [self=this](const any&) { self->AwakenToEmptyFilters(); } });
      }

      CommitToUndo();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // The tool-bar buttons.

   void MainWindow::UpdateTree()
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

      _layersDock->setWindowTitle(QString::fromWCharArray(L::t(L"Filters: ")) + QString::fromWCharArray(_data.TreeFileName.c_str()));

      CommitToUndo();
   }

}
