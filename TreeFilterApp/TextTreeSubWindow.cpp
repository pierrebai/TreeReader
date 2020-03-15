#include "TextTreeSubWindow.h"
#include "TextTreeModel.h"

#include <QtWidgets/qtreeview.h>

namespace TreeReaderApp
{
   TextTreeSubWindow::TextTreeSubWindow(const TextTreePtr& tree, const std::wstring& name)
   : OriginalTree(tree)
   {
      _treeView = new QTreeView;
      _treeView->setUniformRowHeights(true);
      _treeView->setHeaderHidden(true);
      _treeView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

      _model = new TextTreeModel;
      _model->Tree = tree;

      auto oldModel = _treeView->model();
      _treeView->setModel(_model);
      delete oldModel;

      setWidget(_treeView);

      setWindowTitle(QString::fromStdWString(name));
      setAttribute(Qt::WA_DeleteOnClose);
   }

   void TextTreeSubWindow::UpdateShownModel(const TextTreePtr& tree)
   {
      // Note: we don't change the OriginalTree variable, only which tree is shown (original tree or filtered).
      _model->Tree = tree;
      _model->Reset();
   }
}
