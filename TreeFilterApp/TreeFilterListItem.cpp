#include "TreeFilterListItem.h"
#include "TreeFilterListWidget.h"

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
   using namespace Qtadditions;

   namespace
   {
      using DeleteCallbackfunction = TreeFilterListItem::DeleteCallbackfunction;
      using EditCallbackfunction = TreeFilterListItem::EditCallbackfunction;

      /////////////////////////////////////////////////////////////////////////
      //
      // filter panel creation helpers.

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<accept_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<stop_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<stop_when_kept_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<until_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<contains_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, &filter->Contained);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<unique_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<regex_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, &filter->regexTextForm);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<not_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<if_subtree_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<if_sibling_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<or_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<and_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<under_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr, &filter->include_self);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<remove_children_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr, &filter->include_self);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<level_range_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr, nullptr, &filter->min_level, &filter->max_level);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<named_tree_filter>& filter, DeleteCallbackfunction delfunc, EditCallbackfunction editfunc)
      {
         return new TreeFilterListItem(filter, delfunc, editfunc, nullptr);
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // filter panel creation.

   TreeFilterListItem* TreeFilterListItem::Create(
      const tree_filter_ptr& filter,
      DeleteCallbackfunction delfunc,
      EditCallbackfunction editfunc)
   {
      #define CALL_CONVERTER(a) if (auto ptr = dynamic_pointer_cast<a>(filter)) { return CreateFilterPanel(ptr, delfunc, editfunc); }

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

   TreeFilterListItem::TreeFilterListItem(
      const shared_ptr<tree_filter>& filter,
      DeleteCallbackfunction delfunc, EditCallbackfunction editfunc,
      wstring* textContent, bool* includeSelf, size_t* count, size_t* count2)
      : filter(filter)
   {
      const bool active = (delfunc != nullptr);

      setToolTip(QString::fromStdWString(filter->get_description()));
      setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

      auto container_layout = new QVBoxLayout(this);
      container_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
      container_layout->setMargin(4);
      setLayout(container_layout);

      auto name_layout = new QHBoxLayout;
      name_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
      name_layout->setMargin(0);
      container_layout->addLayout(name_layout);

      auto nameLabel = new QLabel(QString::fromStdWString(filter->get_short_name()));
      name_layout->addWidget(nameLabel);

      if (textContent)
      {
         auto textEdit = new QLineEdit(QString::fromStdWString(*textContent));
         textEdit->setMaximumWidth(80);
         textEdit->setEnabled(active);
         name_layout->addWidget(textEdit);

         textEdit->connect(textEdit, &QLineEdit::editingFinished, [textContent, textEdit]()
         {
            *textContent = textEdit->text().toStdWString();
         });
      }

      if (count)
      {
         auto numberEdit = new QLineEdit(QString().setNum(*count));
         numberEdit->setMaximumWidth(count2 ? 40 : 80);
         numberEdit->setValidator(new QIntValidator);
         numberEdit->setEnabled(active);
         name_layout->addWidget(numberEdit);

         numberEdit->connect(numberEdit, &QLineEdit::editingFinished, [count, numberEdit]()
         {
            *count = numberEdit->text().toUInt();
         });
      }

      if (count2)
      {
         auto numberEdit = new QLineEdit(QString().setNum(*count2));
         numberEdit->setMaximumWidth(count ? 40 : 80);
         numberEdit->setValidator(new QIntValidator);
         numberEdit->setEnabled(active);
         name_layout->addWidget(numberEdit);

         numberEdit->connect(numberEdit, &QLineEdit::editingFinished, [count2, numberEdit]()
         {
            *count2 = numberEdit->text().toUInt();
         });
      }

      if (editfunc)
      {
         auto editButton = new QPushButton;
         editButton->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_EDIT)));
         editButton->setToolTip(tr("Edit this filter"));
         editButton->setFlat(true);
         editButton->setMaximumSize(QSize(16, 16));
         name_layout->addWidget(editButton);

         editButton->connect(editButton, &QPushButton::clicked, [filter, self = this, editfunc]()
         {
            editfunc(self);
         });
      }

      if (delfunc)
      {
         auto deleteButton = new QPushButton;
         deleteButton->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_DELETE)));
         deleteButton->setToolTip(tr("Delete this filter"));
         deleteButton->setFlat(true);
         deleteButton->setMaximumSize(QSize(16, 16));
         name_layout->addWidget(deleteButton);

         deleteButton->connect(deleteButton, &QPushButton::clicked, [filter, self = this, delfunc]()
         {
            delfunc(self);
         });
      }

      if (includeSelf)
      {
         auto includeBox = new QCheckBox(tr("Include self"));
         includeBox->setChecked(active);
         container_layout->addWidget(includeBox);

         includeBox->connect(includeBox, &QCheckBox::stateChanged, [includeSelf, includeBox](int)
         {
            *includeSelf = includeBox->isChecked();
         });
      }

      if (auto combine = dynamic_pointer_cast<combine_tree_filter>(filter))
      {
         SubList = new TreeFilterListWidget(delfunc, editfunc, {}, false);
         SubList->setAcceptDrops(true);
         container_layout->addWidget(SubList);
      }
   }

   TreeFilterListItem* TreeFilterListItem::clone() const
   {
      return clone(nullptr, nullptr);
   }

   TreeFilterListItem* TreeFilterListItem::clone(DeleteCallbackfunction delfunc, EditCallbackfunction editfunc) const
   {
      return Create(filter->clone(), delfunc, editfunc);
   }

}

