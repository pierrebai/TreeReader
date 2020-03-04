#include "TreeFilterListWidget.h"
#include "QtUtilities.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qlayout.h>

#include <QtGui/qdrag.h>
#include <QtGui/qpainter.h>
#include <QtGui/qwindow.h>
#include <QtGui/qvalidator.h>

#include <QtGui/qevent.h>

namespace TreeReaderApp
{
   using namespace TreeReader;
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree filter panel.

   TreeFilterListWidget::TreeFilterListWidget(DeleteCallbackFunction callback, QWidget* parent)
   : QWidgetListWidget(callback, parent)
   {
   }

   QWidgetListItem* TreeFilterListWidget::AddTreeFilter(const TreeFilterPtr& filter, int index)
   {
      return AddItem(TreeFilterWidget::Create(filter, DeleteCallback), index);
   }

   QWidgetListItem* TreeFilterListWidget::CloneItem(QWidgetListItem* item) const
   {
      if (auto tfItem = dynamic_cast<TreeFilterWidget *>(item))
         return tfItem->Clone(DeleteCallback);
      else
         return QWidgetListWidget::CloneItem(item);
   }
}

