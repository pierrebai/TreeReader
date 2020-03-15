#include "TextTreeSubWindow.h"
#include "TextTreeModel.h"
#include "TreeCommands.h"

#include "QtUtilities.h"

#include "TreeReaderHelpers.h"

#include <QtWidgets/qtreeview.h>

#include <QtGui/qevent.h>

namespace TreeReaderApp
{
   using namespace TreeReader;
   using namespace QtAdditions;
   using namespace std;

   TextTreeSubWindow::TextTreeSubWindow(const TreeCommandsPtr& tree, CommandsOptions& options)
   : Tree(tree), _options(options)
   {
      _treeView = new QTreeView;
      _treeView->setUniformRowHeights(true);
      _treeView->setHeaderHidden(true);
      _treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

      _model = new TextTreeModel;
      _model->Tree = tree->GetOriginalTree();

      auto oldModel = _treeView->model();
      _treeView->setModel(_model);
      delete oldModel;

      setWidget(_treeView);

      setWindowTitle(QString::fromStdWString(tree->GetOriginalTreeFileName()));
      setAttribute(Qt::WA_DeleteOnClose);
   }

   void TextTreeSubWindow::UpdateShownModel(const TextTreePtr& tree)
   {
      // Note: we don't change the OriginalTree variable, only which tree is shown (original tree or filtered).
      _model->Tree = tree;
      _model->Reset();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Closing and saving.

   void TextTreeSubWindow::closeEvent(QCloseEvent* ev)
   {
      if (SaveIfRequired(tr("close the tab"), tr("closing the tab")))
      {
         WithNoExceptions([self = this]() { self->Tree->AbortAsyncFilter(); });

         QWidget::closeEvent(ev);
      }
      else
      {
         ev->ignore();
      }
   }

   bool TextTreeSubWindow::SaveIfRequired(const QString& action, const QString& actioning)
   {
      if (Tree->GetFilteredTree() && !Tree->IsFilteredTreeSaved())
      {
         YesNoCancel answer = AskYesNoCancel(
            tr("Unsaved Text Tree Warning"),
            QString(tr("The current filtered tree has not been saved.\nDo you want to save it before ")) + actioning + QString(tr("?")),
            this);
         if (answer == YesNoCancel::Cancel)
            return false;
         else if (answer == YesNoCancel::Yes)
            if (!SaveFilteredTree(_options))
               return false;
      }

      return true;
   }

   bool TextTreeSubWindow::SaveFilteredTree(const CommandsOptions& options)
   {
      if (!Tree->GetFilteredTree())
         return true;

      auto path = AskSave(tr("Save Filtered Text Tree"), tr(TreeCommands::TreeFileTypes), "", this);

      Tree->SaveFilteredTree(path, options);

      return true;
   }

}
