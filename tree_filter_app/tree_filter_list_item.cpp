#include "tree_filter_list_item.h"
#include "tree_filter_list_widget.h"

#include "dak/QtAdditions/QtUtilities.h"

#include "dak/tree_reader/named_filters.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qapplication.h>

#include <QtGui/qvalidator.h>
#include <QtGui/qevent.h>

#include "resource.h"

namespace dak::tree_reader::app
{
   using namespace dak::tree_reader;
   using namespace std;
   using namespace QtAdditions;

   namespace
   {
      using delete_callback_function = tree_filter_list_item::delete_callback_function;
      using edit_callback_function = tree_filter_list_item::edit_callback_function;

      /////////////////////////////////////////////////////////////////////////
      //
      // filter panel creation helpers.

      tree_filter_list_item* create_filter_panel(const shared_ptr<accept_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<stop_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<stop_when_kept_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<until_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<contains_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, &filter->contained);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<unique_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<regex_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, &filter->regex_text);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<not_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<if_subtree_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<if_sibling_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<or_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<and_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<under_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr, &filter->include_self);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<remove_children_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr, &filter->include_self);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<level_range_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr, nullptr, &filter->min_level, &filter->max_level);
      }

      tree_filter_list_item* create_filter_panel(const shared_ptr<named_tree_filter>& filter, delete_callback_function delfunc, edit_callback_function editfunc)
      {
         return new tree_filter_list_item(filter, delfunc, editfunc, nullptr);
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // filter panel creation.

   tree_filter_list_item* tree_filter_list_item::create(
      const tree_filter_ptr& filter,
      delete_callback_function delfunc,
      edit_callback_function editfunc)
   {
      #define CALL_CONVERTER(a) if (auto ptr = dynamic_pointer_cast<a>(filter)) { return create_filter_panel(ptr, delfunc, editfunc); }

      CALL_CONVERTER(accept_tree_filter)
      CALL_CONVERTER(stop_tree_filter)
      CALL_CONVERTER(stop_when_kept_tree_filter)
      CALL_CONVERTER(until_tree_filter)
      CALL_CONVERTER(contains_tree_filter)
      CALL_CONVERTER(unique_tree_filter)
      CALL_CONVERTER(regex_tree_filter)
      CALL_CONVERTER(not_tree_filter)
      CALL_CONVERTER(if_subtree_tree_filter)
      CALL_CONVERTER(if_sibling_tree_filter)
      CALL_CONVERTER(or_tree_filter)
      CALL_CONVERTER(and_tree_filter)
      CALL_CONVERTER(under_tree_filter)
      CALL_CONVERTER(remove_children_tree_filter)
      CALL_CONVERTER(level_range_tree_filter)
      CALL_CONVERTER(named_tree_filter)

      #undef CALL_CONVERTER

      return nullptr;
   }

   tree_filter_list_item::tree_filter_list_item(
      const shared_ptr<tree_filter>& a_filter,
      delete_callback_function delfunc, edit_callback_function editfunc,
      wstring* textContent, bool* includeSelf, size_t* count, size_t* count2)
      : filter(a_filter)
   {
      const bool active = (delfunc != nullptr);

      setToolTip(QString::fromStdWString(a_filter->get_description()));
      setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

      auto container_layout = new QVBoxLayout(this);
      container_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
      container_layout->setMargin(4);
      setLayout(container_layout);

      auto name_layout = new QHBoxLayout;
      name_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
      name_layout->setMargin(0);
      container_layout->addLayout(name_layout);

      auto nameLabel = new QLabel(QString::fromStdWString(a_filter->get_short_name()));
      name_layout->addWidget(nameLabel);

      if (textContent)
      {
         auto text_edit = new QLineEdit(QString::fromStdWString(*textContent));
         text_edit->setMaximumWidth(80);
         text_edit->setEnabled(active);
         name_layout->addWidget(text_edit);

         text_edit->connect(text_edit, &QLineEdit::editingFinished, [textContent, text_edit]()
         {
            *textContent = text_edit->text().toStdWString();
         });
      }

      if (count)
      {
         auto number_edit = new QLineEdit(QString().setNum(*count));
         number_edit->setMaximumWidth(count2 ? 40 : 80);
         number_edit->setValidator(new QIntValidator);
         number_edit->setEnabled(active);
         name_layout->addWidget(number_edit);

         number_edit->connect(number_edit, &QLineEdit::editingFinished, [count, number_edit]()
         {
            *count = number_edit->text().toUInt();
         });
      }

      if (count2)
      {
         auto number_edit = new QLineEdit(QString().setNum(*count2));
         number_edit->setMaximumWidth(count ? 40 : 80);
         number_edit->setValidator(new QIntValidator);
         number_edit->setEnabled(active);
         name_layout->addWidget(number_edit);

         number_edit->connect(number_edit, &QLineEdit::editingFinished, [count2, number_edit]()
         {
            *count2 = number_edit->text().toUInt();
         });
      }

      if (editfunc)
      {
         auto edit_button = new QPushButton;
         edit_button->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_EDIT)));
         edit_button->setToolTip(tr("Edit this filter"));
         edit_button->setFlat(true);
         edit_button->setMaximumSize(QSize(16, 16));
         name_layout->addWidget(edit_button);

         edit_button->connect(edit_button, &QPushButton::clicked, [a_filter, self = this, editfunc]()
         {
            editfunc(self);
         });
      }

      if (delfunc)
      {
         auto delete_button = new QPushButton;
         delete_button->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_DELETE)));
         delete_button->setToolTip(tr("Delete this filter"));
         delete_button->setFlat(true);
         delete_button->setMaximumSize(QSize(16, 16));
         name_layout->addWidget(delete_button);

         delete_button->connect(delete_button, &QPushButton::clicked, [a_filter, self = this, delfunc]()
         {
            delfunc(self);
         });
      }

      if (includeSelf)
      {
         auto include_box = new QCheckBox(tr("Include self"));
         include_box->setChecked(active);
         container_layout->addWidget(include_box);

         include_box->connect(include_box, &QCheckBox::stateChanged, [includeSelf, include_box](int)
         {
            *includeSelf = include_box->isChecked();
         });
      }

      if (auto combine = dynamic_pointer_cast<combine_tree_filter>(a_filter))
      {
         sub_list = new tree_filter_list_widget(delfunc, editfunc, {}, false);
         sub_list->setAcceptDrops(true);
         container_layout->addWidget(sub_list);
      }
   }

   tree_filter_list_item* tree_filter_list_item::clone() const
   {
      return clone(nullptr, nullptr);
   }

   tree_filter_list_item* tree_filter_list_item::clone(delete_callback_function delfunc, edit_callback_function editfunc) const
   {
      return create(filter->clone(), delfunc, editfunc);
   }

}

