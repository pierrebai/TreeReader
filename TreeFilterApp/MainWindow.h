#pragma once

#include "FiltersEditor.h"
#include "UndoStack.h"

#include "TreeFilterCommands.h"

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbutton.h>

#include <vector>
#include <map>

namespace TreeReaderApp
{
   using TreeFilter = TreeReader::TreeFilter;
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

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

      int LayerCopy = 0;
      int LayerAdd = 0;
      int LayerDelete = 0;
      int LayerMoveUp = 0;
      int LayerMoveDown = 0;
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
      std::vector<TreeFilterPtr> GetSelectedFilters();
      void UpdateFiltersEditor();
      void FillFiltersEditor();

      // Undo / redo tool-bar buttons.
      void DeadedFilters(std::any& data);
      void UpdateUndoRedoActions();
      void AwakenFilters(const std::any& data);
      void AwakenToEmptyFilters();
      void ClearUndoStack();
      void CommitToUndo();

      // Layer manipulations.
      std::vector<TreeFilterPtr> CloneFilters(const std::vector<TreeFilterPtr>& filters);
      void AddFilter(const TreeFilterPtr& newFilter);

      // The mosaic tool-bar buttons.
      void UpdateFilters(const std::vector<TreeFilterPtr>& layers, const std::wstring& name);

      // Closing and saving.
      void closeEvent(QCloseEvent* ev);
      bool SaveIfRequired(const std::wstring& action, const std::wstring& actioning);
      bool SaveFilteredTree();

      // Data.
      std::vector<std::wstring> errors;
      UndoStack _undoStack;

      TreeReader::CommandsContext ctx;
      std::vector<TreeFilterPtr> filtered;

      // UI elements.
      QAction* _undoAction = nullptr;
      QToolButton* _undoButton = nullptr;

      QAction* _redoAction = nullptr;
      QToolButton* _redoButton = nullptr;

      QAction* _loadTreeAction = nullptr;
      QToolButton* _loadTreeButton = nullptr;

      QAction* _saveTreeAction = nullptr;
      QToolButton* _saveTreeButton = nullptr;

      QDockWidget* _layersDock = nullptr;
      FiltersEditor* _filtersList = nullptr;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
