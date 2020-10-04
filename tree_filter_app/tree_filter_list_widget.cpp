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

   tree_filter_list_widget_t::tree_filter_list_widget_t(
      delete_callback_function_t del_func,
      edit_callback_function_t edit_func,
      ListModifiedCallbackFunction modif_func,
      bool stretch, QWidget* parent)
   : QWidgetListWidget(modif_func, stretch, QBoxLayout::Direction::TopToBottom, parent),
     delete_callback(del_func),
     edit_callback(edit_func)
   {
   }

   QWidgetListItem* tree_filter_list_widget_t::add_tree_filter(const tree_filter_ptr_t& filter, int index)
   {
      return add_tree_filter(filter, delete_callback, edit_callback, index);
   }

   QWidgetListItem* tree_filter_list_widget_t::add_tree_filter(const tree_filter_ptr_t& filter, delete_callback_function_t del_func, edit_callback_function_t edit_func, int index)
   {
      return addItem(tree_filter_list_item_t::create(filter, del_func, edit_func), index);
   }

   vector<tree_filter_ptr_t> tree_filter_list_widget_t::get_tree_filters() const
   {
      vector<tree_filter_ptr_t> filters;

      vector<QWidgetListItem*> widgets = getItems();
      for (auto& wid : widgets)
      {
         if (auto tfw = dynamic_cast<tree_filter_list_item_t*>(wid))
         {
            if (tfw->filter)
               filters.emplace_back(tfw->filter);
         }
      }

      return filters;
   }

   QWidgetListItem* tree_filter_list_widget_t::cloneItem(QWidgetListItem* item) const
   {
      if (auto tf_item = dynamic_cast<tree_filter_list_item_t *>(item))
         return tf_item->clone(delete_callback, edit_callback);
      else
         return QWidgetListWidget::cloneItem(item);
   }
}

