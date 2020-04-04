#pragma once

#include "filter_editor.h"
#include "tree_filter_list_widget.h"

#include "dak/QtAdditions/QWidgetScrollListWidget.h"

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

   struct text_tree_sub_window;

   ////////////////////////////////////////////////////////////////////////////
   //
   // The main window of the tree filter app.

   struct main_window : QMainWindow
   {
      // Create the main window.
      main_window();

   protected:
      // Create the UI elements.
      void build_ui();

      void build_toolbar_ui();
      void build_filters_ui();
      void build_simple_search_ui();
      void build_tabbed_ui();

      // Connect the signals of the UI elements.
      void connect_ui();

      // Fill the UI with the intial data.
      void fill_ui();
      void fill_filter_editor_ui();
      void fill_available_filters_ui();

      // undo / redo tool-bar buttons.
      void update_undo_redo_actions();

      // Closing, loading and saving.
      void closeEvent(QCloseEvent* ev);
      bool save_if_required(const QString& action, const QString& actioning);
      void load_tree();
      bool save_filtered_tree(text_tree_sub_window* window);

      // Tab management.
      void add_text_tree_tab(const tree_commands_ptr& newTree);
      void update_active_tab(); 
      void update_text_tree_tab();

      // Current tab.
      text_tree_sub_window* get_current_sub_window();
      std::vector<text_tree_sub_window*> get_all_sub_windows();

      // Main window state.
      void save_state();
      void load_state();

      // Tree filtering.
      void filter_tree();
      void verify_async_filtering();
      void abort_async_filtering();
      void search_in_tree(const QString& text);
      void search_in_tree();

      void push_filter();
      void update_create_tab_action();

      // filter naming.
      void name_filter();
      void add_named_filter_to_available(const tree_filter_ptr& filter);

      // options.
      void open_options();

      // data.
      global_commands _data;

      // Toolbar buttons.
      QAction* _undo_action = nullptr;
      QToolButton* _undo_button = nullptr;

      QAction* _redo_action = nullptr;
      QToolButton* _redo_button = nullptr;

      QAction* _load_tree_action = nullptr;
      QToolButton* _load_tree_button = nullptr;

      QAction* _save_tree_action = nullptr;
      QToolButton* _save_tree_button = nullptr;

      QAction* _apply_filter_action = nullptr;
      QToolButton* _apply_filter_button = nullptr;

      QAction* _name_filter_action = nullptr;
      QToolButton* _name_filter_button = nullptr;

      QAction* _push_filter_action = nullptr;
      QToolButton* _push_filter_button = nullptr;

      QAction* _options_action = nullptr;
      QToolButton* _options_button = nullptr;

      QPushButton* _edit_search_button = nullptr;

      // UI elements.
      QLineEdit* _simple_search = nullptr;
      filter_editor* _filter_editor = nullptr;
      tree_filter_list_widget* _available_filters_list = nullptr;
      QWidgetScrollListWidget* _available_filters_scroll = nullptr;
      QTimer* _filtering_timer = nullptr;

      QMdiArea* _tabs = nullptr;

      Q_OBJECT;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
