#include "TextTreeSubWindow.h"
#include "TextTreeModel.h"

#include "dak/tree_reader/tree_commands.h"

#include "dak/QtAdditions/QtUtilities.h"

#include "dak/utility/exceptions.h"

#include <QtWidgets/qtreeview.h>

#include <QtGui/qevent.h>

namespace dak::tree_reader::app
{
   using namespace dak::tree_reader;
   using namespace dak::utility;
   using namespace Qtadditions;
   using namespace std;

   TextTreeSubWindow::TextTreeSubWindow(const tree_commands_ptr& tree, commands_options& options)
   : Tree(tree), _options(options)
   {
      _treeView = new QTreeView;
      _treeView->setUniformRowHeights(true);
      _treeView->setHeaderHidden(true);
      _treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

      _model = new TextTreeModel;
      _model->Tree = tree->get_original_tree();

      auto oldModel = _treeView->model();
      _treeView->setModel(_model);
      delete oldModel;

      setWidget(_treeView);

      setWindowTitle(QString::fromStdWString(tree->get_original_tree_filename()));
      setAttribute(Qt::WA_DeleteOnClose);
   }

   void TextTreeSubWindow::UpdateShownModel(const text_tree_ptr& tree)
   {
      // note: we don't change the OriginalTree variable, only which tree is shown (original tree or filtered).
      _model->Tree = tree;
      _model->reset();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Closing and saving.

   void TextTreeSubWindow::closeEvent(QCloseEvent* ev)
   {
      if (SaveIfRequired(tr("close the tab"), tr("closing the tab")))
      {
         with_no_exceptions([self = this]() { self->Tree->abort_async_filter(); });

         QWidget::closeEvent(ev);
      }
      else
      {
         ev->ignore();
      }
   }

   bool TextTreeSubWindow::SaveIfRequired(const QString& action, const QString& actioning)
   {
      if (Tree->get_filtered_tree() && !Tree->is_filtered_tree_saved())
      {
         YesNoCancel answer = AskYesNoCancel(
            tr("Unsaved Text Tree Warning"),
            QString(tr("The current filtered tree has not been saved.\nDo you want to save it before ")) + actioning + QString(tr("?")),
            this);
         if (answer == YesNoCancel::Cancel)
            return false;
         else if (answer == YesNoCancel::Yes)
            if (!save_filtered_tree(_options))
               return false;
      }

      return true;
   }

   bool TextTreeSubWindow::save_filtered_tree(const commands_options& options)
   {
      if (!Tree->get_filtered_tree())
         return true;

      auto path = AskSave(tr("Save Filtered Text Tree"), tr(tree_commands::tree_file_types), "", this);

      Tree->save_filtered_tree(path, options);

      return true;
   }

}
