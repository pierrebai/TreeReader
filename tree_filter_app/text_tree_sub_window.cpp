#include "text_tree_sub_window.h"
#include "text_tree_model.h"

#include "dak/tree_reader/tree_commands.h"

#include "dak/QtAdditions/QtUtilities.h"

#include "dak/utility/exceptions.h"

#include <QtWidgets/qtreeview.h>

#include <QtGui/qevent.h>

namespace dak::tree_reader::app
{
   using namespace dak::tree_reader;
   using namespace dak::utility;
   using namespace QtAdditions;
   using namespace std;

   text_tree_sub_window::text_tree_sub_window(const tree_commands_ptr& a_tree, commands_options& options)
   : tree(a_tree), _options(options)
   {
      _tree_view = new QTreeView;
      _tree_view->setUniformRowHeights(true);
      _tree_view->setHeaderHidden(true);
      _tree_view->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

      _model = new text_tree_model;
      _model->tree = tree->get_original_tree();

      auto old_model = _tree_view->model();
      _tree_view->setModel(_model);
      delete old_model;

      setWidget(_tree_view);

      setWindowTitle(QString::fromStdWString(tree->get_original_tree_filename()));
      setAttribute(Qt::WA_DeleteOnClose);
   }

   void text_tree_sub_window::update_shown_model(const text_tree_ptr& a_tree)
   {
      // note: we don't change the OriginalTree variable, only which tree is shown (original tree or filtered).
      _model->tree = a_tree;
      _model->reset();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Closing and saving.

   void text_tree_sub_window::closeEvent(QCloseEvent* ev)
   {
      if (save_if_required(tr("close the tab"), tr("closing the tab")))
      {
         with_no_exceptions([self = this]() { self->tree->abort_async_filter(); });

         QWidget::closeEvent(ev);
      }
      else
      {
         ev->ignore();
      }
   }

   bool text_tree_sub_window::save_if_required(const QString& action, const QString& actioning)
   {
      if (tree->get_filtered_tree() && !tree->is_filtered_tree_saved())
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

   bool text_tree_sub_window::save_filtered_tree(const commands_options& options)
   {
      if (!tree->get_filtered_tree())
         return true;

      auto path = AskSave(tr("Save Filtered Text Tree"), tr(tree_commands::tree_file_types), "", this);

      tree->save_filtered_tree(path, options);

      return true;
   }

}
