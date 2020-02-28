#include "MainWindow.h"
#include "QtUtilities.h"

#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qtreeview.h>
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

         _filtersList = new FiltersEditor(filters_container, icons.LayerCopy, icons.LayerAdd, icons.LayerDelete, icons.LayerMoveUp, icons.LayerMoveDown);
         filters_layout->addWidget(_filtersList);

         _layersDock->setWidget(filters_container);

      auto treeView = new QTreeView;
      treeView->setUniformRowHeights(true);
      treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

      setCentralWidget(treeView);
      addToolBar(toolbar);
      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, _layersDock);
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
         if (!self->SaveIfRequired(L::t(L"load a mosaic"), L::t(L"loading a mosaic")))
            return;

         filesystem::path path;
         // TODO LOAD TREE
         //auto filters = ask_open_filtered_mosaic(self->known_tilings, path, self);
         //if (filters.size() == 0)
         //   return;
         //self->ClearUndoStack();
         //self->update_mosaic_map(filters, path.filename());
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
         self->UpdateFiltersEditor();
         self->CommitToUndo();
         self->ctx.Filter = filters.empty() ? TreeFilterPtr() : filters.front();
      };

      _filtersList->SelectionChanged = [self=this](const FiltersEditor::Filters& filters)
      {
         // Nothing...
      };

      _filtersList->NewFilterRequested = [self=this]()
      {
         self->AddFilter(TreeReader::Contains(L"TODO"));
      };
   }

   // Fill the UI with the intial data.
   void MainWindow::FillUI()
   {
      // TODO: fill tree view
   }

   void MainWindow::closeEvent(QCloseEvent* ev)
   {
      if (SaveIfRequired(L::t(L"close the window"), L::t(L"closing the window")))
         QWidget::closeEvent(ev);
      else
         ev->ignore();
   }

   bool MainWindow::SaveIfRequired(const wstring& action, const wstring& actioning)
   {
      if (_undoStack.HasUndo())
      {
         YesNoCancel answer = AskYesNoCancel(
            L::t(L"Unsaved Mosaic Warning"),
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

   void MainWindow::UpdateFiltersEditor()
   {
      _filtersList->UpdateListContent();
   }

   void MainWindow::FillFiltersEditor()
   {
      vector<TreeFilterPtr> filters;
      if (ctx.Filter)
         filters.push_back(ctx.Filter);
      _filtersList->SetEdited(filters);
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
      vector<TreeFilterPtr> filters = CloneFilters(any_cast<const vector<TreeFilterPtr>&>(data));

      filtered = filters;

      FillFiltersEditor();
   }

   void MainWindow::AwakenToEmptyFilters()
   {
      filtered.clear();

      FillFiltersEditor();
   }

   void MainWindow::ClearUndoStack()
   {
      _undoStack.Clear();
   }

   void MainWindow::CommitToUndo()
   {
      const vector<TreeFilterPtr>& filters = filtered;
      _undoStack.Commit(
      {
         CloneFilters(filters),
         [self=this](any& data) { self->DeadedFilters(data); },
         [self=this](const any& data) { self->AwakenFilters(data); }
      });
      UpdateUndoRedoActions();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Layer manipulations.

   vector<TreeFilterPtr> MainWindow::CloneFilters(const vector<TreeFilterPtr>& filters)
   {
      vector<TreeFilterPtr> cloned_filters;
      // TODO CLONE FILTER
      //for (const auto& filter : filters)
      //   cloned_filters.emplace_back(filter->clone());
      return cloned_filters;
   }

   void MainWindow::AddFilter(const TreeFilterPtr& newFilter)
   {
      auto& filters = filtered;
      const bool was_empty = (filters.size() <= 0);
      filters.emplace_back(newFilter);
      FillFiltersEditor();

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
   // The mosaic tool-bar buttons.

   void MainWindow::UpdateFilters(const vector<TreeFilterPtr>& filters, const wstring& name)
   {
      _layersDock->setWindowTitle(QString::fromWCharArray(L::t(L"Filters: ")) + QString::fromWCharArray(name.c_str()));

      filtered = filters;

      FillFiltersEditor();
      CommitToUndo();
   }

}
