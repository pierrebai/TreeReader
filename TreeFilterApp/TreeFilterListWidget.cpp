#include "TreeFilterListWidget.h"
#include "TreeFilterDragWidget.h"
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

   TreeFilterListWidget::TreeFilterListWidget(DeleteCallbackFunction delCallback, EditCallbackFunction editCallback, bool stretch, QWidget* parent)
   : QWidgetListWidget(stretch, parent), DeleteCallback(delCallback), EditCallback(editCallback)
   {
   }

   QWidgetListItem* TreeFilterListWidget::AddTreeFilter(const TreeFilterPtr& filter, int index)
   {
      return AddTreeFilter(filter, DeleteCallback, EditCallback, index);
   }

   QWidgetListItem* TreeFilterListWidget::AddTreeFilter(const TreeFilterPtr& filter, DeleteCallbackFunction delCallback, EditCallbackFunction editCallback, int index)
   {
      return AddItem(TreeFilterWidget::Create(filter, delCallback, editCallback), index);
   }

   vector<TreeFilterPtr> TreeFilterListWidget::GetTreeFilters() const
   {
      vector<QWidgetListItem*> widgets = GetItems();

      vector<TreeFilterPtr> filters;
      for (auto& w : widgets)
      {
         if (auto tfw = dynamic_cast<TreeFilterWidget*>(w))
         {
            if (tfw->Filter)
               filters.push_back(tfw->Filter);
            if (tfw->SubList)
            {
               auto sublist = tfw->SubList->GetTreeFilters();
               filters.insert(filters.end(), sublist.begin(), sublist.end());
            }
         }
      }

      return filters;
   }

   QWidgetListItem* TreeFilterListWidget::CloneItem(QWidgetListItem* item) const
   {
      if (auto tfItem = dynamic_cast<TreeFilterWidget *>(item))
         return tfItem->Clone(DeleteCallback, EditCallback);
      else
         return QWidgetListWidget::CloneItem(item);
   }
}

