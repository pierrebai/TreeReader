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

   TreeFilterDragWidget::TreeFilterDragWidget(DeleteCallbackFunction delCallback, EditCallbackFunction editCallback, QWidget* parent)
   : QWidgetDragWidget(parent), DeleteCallback(delCallback), EditCallback(editCallback)
   {
   }

   QWidgetListItem* TreeFilterDragWidget::AddTreeFilter(const TreeFilterPtr& filter, int index)
   {
      return AddTreeFilter(filter, DeleteCallback, EditCallback, index);
   }

   QWidgetListItem* TreeFilterDragWidget::AddTreeFilter(const TreeFilterPtr& filter, DeleteCallbackFunction delCallback, EditCallbackFunction editCallback, int index)
   {
      return AddItem(TreeFilterWidget::Create(filter, delCallback, editCallback), index);
   }

   vector<TreeFilterPtr> TreeFilterDragWidget::GetTreeFilters() const
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

   QWidgetListItem* TreeFilterDragWidget::CloneItem(QWidgetListItem* item) const
   {
      if (auto tfItem = dynamic_cast<TreeFilterWidget *>(item))
         return tfItem->Clone(DeleteCallback, EditCallback);
      else
         return QWidgetDragWidget::CloneItem(item);
   }
}

