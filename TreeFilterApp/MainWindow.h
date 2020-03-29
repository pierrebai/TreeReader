#pragma once

#include "FilterEditor.h"
#include "TreeFilterListWidget.h"
#include "QWidgetScrollListWidget.h"

#include "dak/tree_reader/global_commands.h"

#include <QtWidgets/qmainwindow.h>

class QToolButton;
class QAction;
class QTreeView;
class QDockWidget;
class QTimer;
class QLineEdit;
class QPushButton;
class QMdiArea;

namespace dak::tree_reader
{
   struct text_tree;
   struct tree_commands;
}

namespace dak::tree_reader::app
{
   using tree_filter = tree_reader::tree_filter;
   using tree_filter_ptr = tree_reader::tree_filter_ptr;
   using text_tree_ptr = tree_reader::text_tree_ptr;
   using global_commands = tree_reader::global_commands;
   using undo_stack = tree_reader::undo_stack;
   using tree_commands_ptr = std::shared_ptr<tree_reader::tree_commands>;

   using QWidgetScrollListWidget = Qtadditions::QWidgetScrollListWidget;

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

      // undo / redo tool-bar buttons.
      void Updateundo_redoActions();

      // Closing, loading and saving.
      void closeEvent(QCloseEvent* ev);
      bool SaveIfRequired(const QString& action, const QString& actioning);
      void load_tree();
      bool save_filtered_tree(TextTreeSubWindow* window);

      // Tab management.
      void addTextTreeTab(const tree_commands_ptr& newTree);
      void UpdateActiveTab(); 
      void UpdateTextTreeTab();

      // Current tab.
      TextTreeSubWindow* getCurrentSubWindow();
      std::vector<TextTreeSubWindow*> getallSubWindows();

      // Main window state.
      void SaveState();
      void LoadState();

      // Tree filtering.
      void filter_tree();
      void VerifyAsyncFiltering();
      void abort_async_filtering();
      void search_in_tree(const QString& text);
      void search_in_tree();

      void PushFilter();
      void UpdateCreateTabAction();

      // filter naming.
      void name_filter();
      void addNamedFilterToAvailable(const tree_filter_ptr& filter);

      // options.
      void OpenOptions();

      // data.
      global_commands _data;

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
