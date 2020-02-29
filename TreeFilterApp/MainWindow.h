#pragma once

#include "FiltersEditor.h"
#include "UndoStack.h"

#include "TreeFilterCommands.h"

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qtreeview.h>
#include <QtWidgets/qlineedit.h>

#include <vector>
#include <map>

namespace TreeReaderApp
{
   using TreeFilter = TreeReader::TreeFilter;
   using TreeFilterPtr = TreeReader::TreeFilterPtr;
   using CommandsContext = TreeReader::CommandsContext;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Main window icon IDs.

   struct MainWindowIcons
   {
      int AppIcon = 0;

      int Undo = 0;
      int Redo = 0;

      int TextTreeOpen = 0;
      int TextTreeSave = 0;

      int FilterCopy = 0;
      int FilterAdd = 0;
      int FilterDelete = 0;
      int FilterMoveUp = 0;
      int FilterMoveDown = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // The main window of Alhambra.

   class MainWindow : public QMainWindow
   {
   public:
      // Create the main window.
      MainWindow(const MainWindowIcons& icons);

   protected:
      // Create the UI elements.
      void BuildUI(const MainWindowIcons& icons);

      // Connect the signals of the UI elements.
      void ConnectUI(const MainWindowIcons& icons);

      // Fill the UI with the intial data.
      void FillUI();

      // The layers UI call-backs.
      void FillFiltersEditor();

      // Undo / redo tool-bar buttons.
      void DeadedFilters(std::any& data);
      void UpdateUndoRedoActions();
      void AwakenFilters(const std::any& data);
      void AwakenToEmptyFilters();
      void ClearUndoStack();
      void CommitToUndo();

      // Layer manipulations.
      TreeFilterPtr CloneFilters(const TreeFilterPtr& filters);
      void RequestNewFilter();

      void UpdateTree();

      // Closing and saving.
      void closeEvent(QCloseEvent* ev);
      bool SaveIfRequired(const std::wstring& action, const std::wstring& actioning);
      bool SaveFilteredTree();

      // Data.
      CommandsContext _data;
      UndoStack _undoStack;

      // UI elements.
      QAction* _undoAction = nullptr;
      QToolButton* _undoButton = nullptr;

      QAction* _redoAction = nullptr;
      QToolButton* _redoButton = nullptr;

      QAction* _loadTreeAction = nullptr;
      QToolButton* _loadTreeButton = nullptr;

      QAction* _saveTreeAction = nullptr;
      QToolButton* _saveTreeButton = nullptr;

      QTreeView* _treeView = nullptr;
      QDockWidget* _layersDock = nullptr;
      QDockWidget* _cmdDock = nullptr;
      QLineEdit* _cmdLine = nullptr;
      FiltersEditor* _filtersList = nullptr;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
