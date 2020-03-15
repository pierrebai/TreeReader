#pragma once

#include "TextTree.h"

#include <QtWidgets/qmdisubwindow.h>

class QTreeView;

namespace TreeReaderApp
{
   struct TextTreeModel;

   using TextTreePtr = TreeReader::TextTreePtr;

   /////////////////////////////////////////////////////////////////////////
   //
   // A MDI sub-window for the text tree.

   struct TextTreeSubWindow : QMdiSubWindow
   {
      // The tree shown, before any filtering.
      TextTreePtr OriginalTree;

      TextTreeSubWindow(const TextTreePtr& tree, const std::wstring& name);

      void UpdateShownModel(const TextTreePtr& tree);

   private:
      QTreeView* _treeView;
      TextTreeModel* _model;

      Q_OBJECT;
   };
}
