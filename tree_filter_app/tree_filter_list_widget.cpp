#include "tree_filter_list_widget.h"

#include "dak/QtAdditions/QtUtilities.h"

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

   tree_filter_list_widget::tree_filter_list_widget(
      delete_callback_function del_func,
      edit_callback_function edit_func,
      ListModifiedCallbackFunction modif_func,
      bool stretch, QWidget* parent)
   : QWidgetListWidget(modif_func, stretch, QBoxLayout::Direction::TopToBottom, parent),
     delete_callback(del_func),
     edit_callback(edit_func)
   {
   }

   QWidgetListItem* tree_filter_list_widget::add_tree_filter(const tree_filter_ptr& filter, int index)
   {
      return add_tree_filter(filter, delete_callback, edit_callback, index);
   }

   QWidgetListItem* tree_filter_list_widget::add_tree_filter(const tree_filter_ptr& filter, delete_callback_function del_func, edit_callback_function edit_func, int index)
   {
      return addItem(tree_filter_list_item::create(filter, del_func, edit_func), index);
   }

   vector<tree_filter_ptr> tree_filter_list_widget::get_tree_filters() const
   {
      vector<tree_filter_ptr> filters;

      vector<QWidgetListItem*> widgets = getItems();
      for (auto& wid : widgets)
      {
         if (auto tfw = dynamic_cast<tree_filter_list_item*>(wid))
         {
            if (tfw->filter)
               filters.emplace_back(tfw->filter);
         }
      }

      return filters;
   }

   QWidgetListItem* tree_filter_list_widget::cloneItem(QWidgetListItem* item) const
   {
      if (auto tf_item = dynamic_cast<tree_filter_list_item *>(item))
         return tf_item->clone(delete_callback, edit_callback);
      else
         return QWidgetListWidget::cloneItem(item);
   }
}

