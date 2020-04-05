#include "main_window.h"
#include "text_tree_model.h"
#include "text_tree_sub_window.h"
#include "options_dialog.h"

#include "dak/QtAdditions/QtUtilities.h"

#include "dak/tree_reader/tree_filter_maker.h"
#include "dak/tree_reader/tree_commands.h"

#include "dak/utility/exceptions.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qmdiarea.h>

#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

#include <QtWinExtras/qwinfunctions.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qtimer.h>

#include <fstream>
#include <iomanip>

#include "resource.h"

namespace dak::tree_reader::app
{
   using namespace dak::tree_reader;
   using namespace dak::utility;
   using namespace QtAdditions;
   using namespace std;

   namespace
   {
      static filesystem::path get_local_data_filename(const QString& filename)
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

      static filesystem::path get_filters_filename()
      {
         return get_local_data_filename("tree-reader-named-filters.txt");
      }

      static filesystem::path get_options_filename()
      {
         return get_local_data_filename("tree-reader-options.txt");
      }

      static filesystem::path get_main_window_state_filename()
      {
         return get_local_data_filename("main-window-state.txt");
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the main window.

   main_window::main_window()
   {
      build_ui();
      fill_ui();
      connect_ui();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the UI elements.

   void main_window::build_ui()
   {
      setCorner(Qt::Corner::TopLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
      setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
      setCorner(Qt::Corner::TopRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);
      setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);

      _filtering_timer = new QTimer(this);
      _filtering_timer->setSingleShot(true);

      build_toolbar_ui();
      build_filters_ui();
      build_simple_search_ui();
      build_tabbed_ui();

      setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 256, 256, 0))));
   }

   void main_window::build_toolbar_ui()
   {
      auto toolbar = new QToolBar();
      toolbar->setObjectName("Main Toolbar");
      toolbar->setIconSize(QSize(32, 32));

      _load_tree_action = CreateAction(tr("Load Tree"), IDB_TREE_OPEN, QKeySequence(QKeySequence::StandardKey::Open));
      _load_tree_button = CreateToolButton(_load_tree_action);
      toolbar->addWidget(_load_tree_button);

      _save_tree_action = CreateAction(tr("Save Tree"), IDB_TREE_SAVE, QKeySequence(QKeySequence::StandardKey::Save));
      _save_tree_button = CreateToolButton(_save_tree_action);
      toolbar->addWidget(_save_tree_button);

      _apply_filter_action = CreateAction(tr("filter Tree"), IDB_FILTER_APPLY, QKeySequence(QKeySequence::StandardKey::Find));
      _apply_filter_button = CreateToolButton(_apply_filter_action);
      toolbar->addWidget(_apply_filter_button);

      _name_filter_action = CreateAction(tr("Name filter"), IDB_FILTER_NAME);
      _name_filter_button = CreateToolButton(_name_filter_action);
      toolbar->addWidget(_name_filter_button);

      _push_filter_action = CreateAction(tr("Use Filtered"), IDB_FILTER_PUSH);
      _push_filter_button = CreateToolButton(_push_filter_action);
      toolbar->addWidget(_push_filter_button);

      toolbar->addSeparator();

      _undo_action = CreateAction(tr("undo"), IDB_UNDO, QKeySequence(QKeySequence::StandardKey::Undo));
      _undo_button = CreateToolButton(_undo_action);
      _undo_action->setEnabled(false);
      toolbar->addWidget(_undo_button);

      _redo_action = CreateAction(tr("redo"), IDB_REDO, QKeySequence(QKeySequence::StandardKey::Redo));
      _redo_button = CreateToolButton(_redo_action);
      _redo_action->setEnabled(false);
      toolbar->addWidget(_redo_button);

      toolbar->addSeparator();

      _options_action = CreateAction(tr("options"), IDB_OPTIONS);
      _options_button = CreateToolButton(_options_action);
      toolbar->addWidget(_options_button);

      addToolBar(toolbar);
   }

   void main_window::build_filters_ui()
   {
      auto filters_dock = new QDockWidget(tr("Tree filter"));
      filters_dock->setObjectName("Tree filter");
      filters_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      auto filters_container = new QWidget();
      auto filters_layout = new QHBoxLayout(filters_container);

      _available_filters_list = new tree_filter_list_widget;
      _available_filters_scroll = new QWidgetScrollListWidget(_available_filters_list);
      filters_layout->addWidget(_available_filters_scroll);

      _filter_editor = new filter_editor(_data.get_Filters(), _data.undo_redo(), filters_container);
      filters_layout->addWidget(_filter_editor);

      filters_dock->setWidget(filters_container);

      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, filters_dock);
   }

   void main_window::build_simple_search_ui()
   {
      auto simple_search_dock = new QDockWidget(tr("Tree Text Search"));
      simple_search_dock->setObjectName("Tree Text Search");
      simple_search_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      auto search_container = new QWidget();
      auto search_layout = new QHBoxLayout(search_container);

      _simple_search = new QLineEdit;
      search_layout->addWidget(_simple_search);

      _edit_search_button = new QPushButton;
      _edit_search_button->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_EDIT)));
      _edit_search_button->setToolTip(tr("Edit Search filter"));
      _edit_search_button->setFlat(true);
      _edit_search_button->setMaximumSize(QSize(16, 16));

      search_layout->addWidget(_edit_search_button);

      simple_search_dock->setWidget(search_container);

      addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, simple_search_dock);
   }

   void main_window::build_tabbed_ui()
   {
      auto main_container = new QWidget;
      auto main_layout = new QVBoxLayout(main_container);

      _tabs = new QMdiArea;
      _tabs->setViewMode(QMdiArea::ViewMode::TabbedView);
      _tabs->setDocumentMode(true);
      _tabs->setTabsClosable(true);

      main_layout->addWidget(_tabs);

      setCentralWidget(main_container);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Connect the signals of the UI elements.

   void main_window::connect_ui()
   {
      /////////////////////////////////////////////////////////////////////////
      //
      // Asynchronous filtering.

      _filtering_timer->connect(_filtering_timer, &QTimer::timeout, [self = this]()
      {
         self->verify_async_filtering();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Simple text search.

      _edit_search_button->connect(_edit_search_button, &QPushButton::clicked, [self = this]()
      {
         auto filter = convert_simple_text_to_filter(self->_simple_search->text().toStdWString(), self->_data.get_Filters());
         if (filter)
         {
            self->_filter_editor->set_edited(filter, L"");
         }
      });

      _simple_search->connect(_simple_search, &QLineEdit::textChanged, [self = this](const QString& text)
      {
         self->search_in_tree(text);
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // undo / redo actions.

      _data.undo_redo().changed = [self = this](undo_stack&)
      {
         self->update_undo_redo_actions();
      };

      _undo_action->connect(_undo_action, &QAction::triggered, [self=this]()
      {
         self->_data.undo_redo().undo();
      });

      _redo_action->connect(_redo_action, &QAction::triggered, [self = this]()
      {
         self->_data.undo_redo().redo();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Load / save actions.

      _load_tree_action->connect(_load_tree_action, &QAction::triggered, [self=this]()
      {
         self->load_tree();
      });

      _save_tree_action->connect(_save_tree_action, &QAction::triggered, [self=this]()
      {
         self->save_filtered_tree(self->get_current_sub_window());
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // Filtering.

      _apply_filter_action->connect(_apply_filter_action, &QAction::triggered, [self = this]()
      {
         self->filter_tree();
      });

      _name_filter_action->connect(_name_filter_action, &QAction::triggered, [self = this]()
      {
         self->name_filter();
      });

      _push_filter_action->connect(_push_filter_action, &QAction::triggered, [self = this]()
      {
         self->push_filter();
      });

      _options_action->connect(_options_action, &QAction::triggered, [self = this]()
      {
         self->open_options();
      });

      /////////////////////////////////////////////////////////////////////////
      //
      // The filter list UI call-backs.

      _filter_editor->filter_changed = [self=this](const tree_filter_ptr& filter)
      {
         auto window = self->get_current_sub_window();
         if (!window)
            return;

         window->tree->set_filter(self->_filter_editor->get_edited());
      };

      /////////////////////////////////////////////////////////////////////////
      //
      // Tabs.

      _tabs->connect(_tabs, &QMdiArea::subWindowActivated, [self = this](QMdiSubWindow* sub)
      {
         self->update_active_tab();
      });
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Fill the UI with the intial data.

   void main_window::fill_ui()
   {
      with_no_exceptions([self = this]() { self->_data.load_named_filters(get_filters_filename()); });
      with_no_exceptions([self = this]() { self->_data.load_options(get_options_filename()); });
      with_no_exceptions([self = this]() { self->load_state(); });

      update_create_tab_action();
      update_undo_redo_actions();

      fill_available_filters_ui();
   }

   void main_window::fill_filter_editor_ui()
   {
      auto window = get_current_sub_window();
      if (!window)
         return;

      _filter_editor->set_edited(window->tree->get_filter(), window->tree->get_filter_name());
   }

   void main_window::fill_available_filters_ui()
   {
      for (const auto& [name, filter] : _data.get_Filters().all())
         add_named_filter_to_available(filter);

      _available_filters_list->add_tree_filter(accept());
      _available_filters_list->add_tree_filter(stop());
      _available_filters_list->add_tree_filter(stop_when_kept(nullptr));
      _available_filters_list->add_tree_filter(until(nullptr));
      _available_filters_list->add_tree_filter(contains(L""));
      _available_filters_list->add_tree_filter(regex(L""));
      _available_filters_list->add_tree_filter(unique());
      _available_filters_list->add_tree_filter(not(nullptr));
      _available_filters_list->add_tree_filter(any(vector<tree_filter_ptr>()));
      _available_filters_list->add_tree_filter(all(vector<tree_filter_ptr>()));
      _available_filters_list->add_tree_filter(under(nullptr));
      _available_filters_list->add_tree_filter(no_child(nullptr));
      _available_filters_list->add_tree_filter(level_range(0, 100));
      _available_filters_list->add_tree_filter(if_subtree(nullptr));
      _available_filters_list->add_tree_filter(if_sibling(nullptr));
   }

   void main_window::add_named_filter_to_available(const tree_filter_ptr& filter)
   {
      auto del_func = [self = this](tree_filter_list_item* panel)
      {
         if (!panel)
            return;

         if (!panel->filter)
            return;

         if (self->_data.remove_Filter(panel->filter->get_name()))
         {
            self->_available_filters_list->removeItem(panel);
         }
      };

      auto edit_func = [self = this](tree_filter_list_item* panel)
      {
         if (!panel)
            return;

         if (auto named = dynamic_pointer_cast<named_tree_filter>(panel->filter))
         {
            if (named->filter)
            {
               self->_filter_editor->set_edited(named->filter->clone(), named->name, true);
            }
         }
      };

      _available_filters_list->add_tree_filter(filter, del_func, edit_func);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Main window state.

   void main_window::save_state()
   {
      ofstream stream(get_main_window_state_filename());
      QByteArray state = saveState();
      stream << state.toBase64().toStdString();
   }

   void main_window::load_state()
   {
      ifstream stream(get_main_window_state_filename());
      string text;
      stream >> text;
      QByteArray state = QByteArray::fromBase64(QByteArray::fromStdString(text));
      restoreState(state, 0);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Closing and saving.

   void main_window::closeEvent(QCloseEvent* ev)
   {
      if (save_if_required(tr("close the window"), tr("closing the window")))
      {
         with_no_exceptions([self = this]() { self->_data.save_named_filters(get_filters_filename()); });
         with_no_exceptions([self = this]() { self->_data.save_options(get_options_filename()); });
         with_no_exceptions([self = this]() { self->save_state(); });
         with_no_exceptions([self = this]() { self->abort_async_filtering(); });

         QWidget::closeEvent(ev);
      }
      else
      {
         ev->ignore();
      }
   }

   bool main_window::save_if_required(const QString& action, const QString& actioning)
   {
      for (auto window : get_all_sub_windows())
         if (!window->save_if_required(action, actioning))
            return false;

      return true;
   }

   void main_window::load_tree()
   {
      filesystem::path path = AskOpen(tr("Load Text Tree"), tr(tree_commands::tree_file_types), this);
      auto new_tree = _data.load_tree(path);
      add_text_tree_tab(new_tree);
      search_in_tree();
   }

   bool main_window::save_filtered_tree(text_tree_sub_window* window)
   {
      if (!window)
         return true;

      return window->save_filtered_tree(_data.options);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tab management.

   void main_window::add_text_tree_tab(const tree_commands_ptr& new_tree)
   {
      if (!new_tree)
         return;

      auto sub_window = new text_tree_sub_window(new_tree, _data.options);
      _tabs->addSubWindow(sub_window);
      sub_window->showMaximized();

      fill_filter_editor_ui();
      search_in_tree();

      update_create_tab_action();
   }

   void main_window::update_active_tab()
   {
      auto window = get_current_sub_window();
      if (!window)
         return;

      fill_filter_editor_ui();
      search_in_tree();
   }

   void main_window::update_text_tree_tab()
   {
      auto window = get_current_sub_window();
      if (!window)
         return;

      text_tree_ptr new_tree;
      if (window->tree->get_filtered_tree())
      {
         new_tree = window->tree->get_filtered_tree();
      }
      else
      {
         new_tree = window->tree->get_original_tree();
      }

      window->update_shown_model(new_tree);

      update_create_tab_action();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Current tab.

   text_tree_sub_window* main_window::get_current_sub_window()
   {
      return dynamic_cast<text_tree_sub_window*>(_tabs->currentSubWindow());
   }

   vector<text_tree_sub_window*> main_window::get_all_sub_windows()
   {
      vector<text_tree_sub_window*> subs;

      for (auto window : _tabs->subWindowList())
         if (auto treeWindow = dynamic_cast<text_tree_sub_window*>(window))
            subs.push_back(treeWindow);

      return subs;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree filtering.

   void main_window::filter_tree()
   {
      auto window = get_current_sub_window();
      if (!window)
         return;

      window->tree->set_filter(_filter_editor->get_edited());

      if (window->tree->get_original_tree() == nullptr)
         return;

      window->tree->apply_filter_to_tree_async();
      _filtering_timer->start(10);
   }

   void main_window::verify_async_filtering()
   {
      auto window = get_current_sub_window();
      if (!window)
         return;

      if (window->tree->is_async_filter_ready())
      {
         update_text_tree_tab();
      }
      else
      {
         _filtering_timer->start(10);
      }
   }

   void main_window::abort_async_filtering()
   {
      for (auto window : get_all_sub_windows())
         window->tree->abort_async_filter();
   }

   void main_window::search_in_tree()
   {
      search_in_tree(_simple_search->text());
   }

   void main_window::search_in_tree(const QString& text)
   {
      auto window = get_current_sub_window();
      if (!window)
         return;

      window->tree->search_in_tree_async(text.toStdWString());
      _filtering_timer->start(10);
   }

   void main_window::push_filter()
   {
      auto window = get_current_sub_window();
      if (!window)
         return;

      auto new_tree = _data.create_tree_from_filtered(window->tree);
      add_text_tree_tab(new_tree);
   }

   void main_window::update_create_tab_action()
   {
      auto window = get_current_sub_window();
      if (!window)
         return;

      _push_filter_action->setEnabled(window->tree->can_create_tree_from_filtered());
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // filter naming.

   void main_window::name_filter()
   {
      auto filter = _filter_editor->get_edited();
      if (!filter)
         return;

      wstring filter_name = _filter_editor->get_edited_name();

      filter_name = AskForText(tr("Name a filter"), tr("filter Name"), QString::fromStdWString(filter_name), this);
      if (filter_name.empty())
         return;

      auto named_filter = _data.name_filter(filter_name, filter->clone());
      add_named_filter_to_available(named_filter);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // options.

   void main_window::open_options()
   {
      auto dialog = new options_dialog(_data.options, this);
      dialog->exec();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // undo/redo.
   void main_window::update_undo_redo_actions()
   {
      _undo_action->setEnabled(_data.undo_redo().has_undo());
      _redo_action->setEnabled(_data.undo_redo().has_redo());
   }

}
