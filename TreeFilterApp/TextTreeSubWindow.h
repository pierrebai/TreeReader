#pragma once

#include "TextTree.h"

#include <QtWidgets/qmdisubwindow.h>

class QTreeView;

namespace TreeReader
{
   struct TreeCommands;
   struct TextTree;
   struct CommandsOptions;
}

namespace TreeReaderApp
{
   using TreeCommandsPtr = std::shared_ptr<TreeReader::TreeCommands>;
   using TextTreePtr = std::shared_ptr<TreeReader::TextTree>;
   using CommandsOptions = TreeReader::CommandsOptions;
   struct TextTreeModel;

   /////////////////////////////////////////////////////////////////////////
   //
   // A MDI sub-window for the text tree.

   struct TextTreeSubWindow : QMdiSubWindow
   {
      // The tree shown, before any filtering.
      TreeCommandsPtr Tree;

      // Create a sub-window for the text tree.
      TextTreeSubWindow(const TreeCommandsPtr& tree, CommandsOptions& options);

      // Update which tree is shown: original or filtered.
      void UpdateShownModel(const TextTreePtr& tree);

      // Saving.
      bool SaveIfRequired(const QString& action, const QString& actioning);
      bool SaveFilteredTree(const CommandsOptions& options);

   private:
      void closeEvent(QCloseEvent* ev);

      CommandsOptions& _options;
      QTreeView* _treeView;
      TextTreeModel* _model;

      Q_OBJECT;
   };
}
