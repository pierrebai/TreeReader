#pragma once

#include "dak/tree_reader/text_tree.h"

#include <QtWidgets/qmdisubwindow.h>

class QTreeView;

namespace dak::tree_reader
{
   struct tree_commands_t;
   struct text_tree_t;
   struct commands_options_t;
}

namespace dak::tree_reader::app
{
   using tree_commands_ptr_t = std::shared_ptr<tree_reader::tree_commands_t>;
   using text_tree_ptr_t = std::shared_ptr<tree_reader::text_tree_t>;
   using commands_options_t = tree_reader::commands_options_t;
   struct text_tree_model_t;

   /////////////////////////////////////////////////////////////////////////
   //
   // A MDI sub-window for the text tree.

   struct text_tree_sub_window_t : QMdiSubWindow
   {
      // The tree shown, before any filtering.
      tree_commands_ptr_t tree;

      // Create a sub-window for the text tree.
      text_tree_sub_window_t(const tree_commands_ptr_t& tree, commands_options_t& options);

      // Update which tree is shown: original or filtered.
      void update_shown_model(const text_tree_ptr_t& tree);

      // Saving.
      bool save_if_required(const QString& action, const QString& actioning);
      bool save_filtered_tree(const commands_options_t& options);

   private:
      void closeEvent(QCloseEvent* ev);

      commands_options_t& _options;
      QTreeView* _tree_view;
      text_tree_model_t* _model;

      Q_OBJECT;
   };
}
