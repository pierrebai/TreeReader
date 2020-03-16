#pragma once

#include "FilterEditor.h"
#include "TreeFilterListWidget.h"
#include "QWidgetScrollListWidget.h"

#include "GlobalCommands.h"

#include <QtWidgets/qmainwindow.h>

class QToolButton;
class QAction;
class QTreeView;
class QDockWidget;
class QTimer;
class QLineEdit;
class QPushButton;
class QMdiArea;

namespace TreeReader
{
   struct TextTree;
   struct TreeCommands;
}

namespace TreeReaderApp
{
   using TreeFilter = TreeReader::TreeFilter;
   using TreeFilterPtr = TreeReader::TreeFilterPtr;
   using TextTreePtr = TreeReader::TextTreePtr;
   using GlobalCommands = TreeReader::GlobalCommands;
   using UndoStack = TreeReader::UndoStack;
   using TreeCommandsPtr = std::shared_ptr<TreeReader::TreeCommands>;

   using QWidgetScrollListWidget = QtAdditions::QWidgetScrollListWidget;

   struct TextTreeSubWindow;

   ////////////////////////////////////////////////////////////////////////////
   //
   // The main window of the tree filter app.

   struct MainWindow : QMainWindow
   {
      // Create the main window.
      MainWindow();

   protected:
      // Create the UI elements.
      void BuildUI();

      void BuildToolBarUI();
      void BuildFiltersUI();
      void BuildSimpleSearchUI();
      void BuildTabbedUI();

      // Connect the signals of the UI elements.
      void ConnectUI();

      // Fill the UI with the intial data.
      void FillUI();
      void FillFilterEditorUI();
      void FillAvailableFiltersUI();

      // Undo / redo tool-bar buttons.
      void UpdateUndoRedoActions();

      // Closing, loading and saving.
      void closeEvent(QCloseEvent* ev);
      bool SaveIfRequired(const QString& action, const QString& actioning);
      void LoadTree();
      bool SaveFilteredTree(TextTreeSubWindow* window);

      // Tab management.
      void AddTextTreeTab(const TreeCommandsPtr& newTree);
      void UpdateActiveTab(); 
      void UpdateTextTreeTab();

      // Current tab.
      TextTreeSubWindow* GetCurrentSubWindow();
      std::vector<TextTreeSubWindow*> GetAllSubWindows();

      // Main window state.
      void SaveState();
      void LoadState();

      // Tree filtering.
      void FilterTree();
      void VerifyAsyncFiltering();
      void AbortAsyncFiltering();
      void SearchInTree(const QString& text);
      void SearchInTree();

      void PushFilter();
      void UpdateCreateTabAction();

      // Filter naming.
      void NameFilter();
      void AddNamedFilterToAvailable(const TreeFilterPtr& filter);

      // Options.
      void OpenOptions();

      // Data.
      GlobalCommands _data;

      // Toolbar buttons.
      QAction* _undoAction = nullptr;
      QToolButton* _undoButton = nullptr;

      QAction* _redoAction = nullptr;
      QToolButton* _redoButton = nullptr;

      QAction* _loadTreeAction = nullptr;
      QToolButton* _loadTreeButton = nullptr;

      QAction* _saveTreeAction = nullptr;
      QToolButton* _saveTreeButton = nullptr;

      QAction* _applyFilterAction = nullptr;
      QToolButton* _applyFilterButton = nullptr;

      QAction* _nameFilterAction = nullptr;
      QToolButton* _nameFilterButton = nullptr;

      QAction* _pushFilterAction = nullptr;
      QToolButton* _pushFilterButton = nullptr;

      QAction* _optionsAction = nullptr;
      QToolButton* _optionsButton = nullptr;

      QPushButton* _editSearchButton = nullptr;

      // UI elements.
      QLineEdit* _simpleSearch = nullptr;
      FilterEditor* _filterEditor = nullptr;
      TreeFilterListWidget* _availableFiltersList = nullptr;
      QWidgetScrollListWidget* _scrollFiltersList = nullptr;
      QTimer* _filteringTimer = nullptr;

      QMdiArea* _tabs = nullptr;

      Q_OBJECT;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
