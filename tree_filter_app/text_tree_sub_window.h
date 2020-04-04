#pragma once

#include "dak/tree_reader/text_tree.h"

#include <QtWidgets/qmdisubwindow.h>

class QTreeView;

namespace dak::tree_reader
{
   struct tree_commands;
   struct text_tree;
   struct commands_options;
}

namespace dak::tree_reader::app
{
   using tree_commands_ptr = std::shared_ptr<tree_reader::tree_commands>;
   using text_tree_ptr = std::shared_ptr<tree_reader::text_tree>;
   using commands_options = tree_reader::commands_options;
   struct text_tree_model;

   /////////////////////////////////////////////////////////////////////////
   //
   // A MDI sub-window for the text tree.

   struct text_tree_sub_window : QMdiSubWindow
   {
      // The tree shown, before any filtering.
      tree_commands_ptr tree;

      // Create a sub-window for the text tree.
      text_tree_sub_window(const tree_commands_ptr& tree, commands_options& options);

      // Update which tree is shown: original or filtered.
      void update_shown_model(const text_tree_ptr& tree);

      // Saving.
      bool save_if_required(const QString& action, const QString& actioning);
      bool save_filtered_tree(const commands_options& options);

   private:
      void closeEvent(QCloseEvent* ev);

      commands_options& _options;
      QTreeView* _tree_view;
      text_tree_model* _model;

      Q_OBJECT;
   };
}
