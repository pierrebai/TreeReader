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

namespace dak::tree_reader::app
{
   using namespace dak::tree_reader;
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Tree filter panel.

   TreeFilterListWidget::TreeFilterListWidget(
      DeleteCallbackfunction delCallback,
      EditCallbackfunction editCallback,
      ListModifiedCallbackfunction modifCallback,
      bool stretch, QWidget* parent)
   : QWidgetListWidget(modifCallback, stretch, parent),
     DeleteCallback(delCallback),
     EditCallback(editCallback)
   {
   }

   QWidgetListItem* TreeFilterListWidget::addTreeFilter(const tree_filter_ptr& filter, int index)
   {
      return addTreeFilter(filter, DeleteCallback, EditCallback, index);
   }

   QWidgetListItem* TreeFilterListWidget::addTreeFilter(const tree_filter_ptr& filter, DeleteCallbackfunction delCallback, EditCallbackfunction editCallback, int index)
   {
      return addItem(TreeFilterListItem::Create(filter, delCallback, editCallback), index);
   }

   vector<tree_filter_ptr> TreeFilterListWidget::getTreeFilters() const
   {
      vector<tree_filter_ptr> filters;

      vector<QWidgetListItem*> widgets = getItems();
      for (auto& w : widgets)
      {
         if (auto tfw = dynamic_cast<TreeFilterListItem*>(w))
         {
            if (tfw->filter)
               filters.emplace_back(tfw->filter);
         }
      }

      return filters;
   }

   QWidgetListItem* TreeFilterListWidget::cloneItem(QWidgetListItem* item) const
   {
      if (auto tfItem = dynamic_cast<TreeFilterListItem *>(item))
         return tfItem->clone(DeleteCallback, EditCallback);
      else
         return QWidgetListWidget::cloneItem(item);
   }
}

