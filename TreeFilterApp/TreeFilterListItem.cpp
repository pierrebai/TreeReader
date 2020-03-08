#include "TreeFilterListItem.h"
#include "TreeFilterListWidget.h"
#include "QtUtilities.h"

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

namespace TreeReaderApp
{
   using namespace TreeReader;
   using namespace std;
   using namespace QtAdditions;

   namespace
   {
      using DeleteCallbackFunction = TreeFilterListItem::DeleteCallbackFunction;
      using EditCallbackFunction = TreeFilterListItem::EditCallbackFunction;

      namespace L
      {
         inline const wchar_t* t(const wchar_t* text)
         {
            return text;
         }
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // Filter panel creation helpers.

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<AcceptTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<StopTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<UntilTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<ContainsTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, &filter->Contained);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<RegexTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, &filter->RegexTextForm);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<NotTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<IfSubTreeTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<IfSiblingTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<CountChildrenTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr, &filter->IncludeSelf, &filter->Count);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<CountSiblingsTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr, &filter->IncludeSelf, &filter->Count);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<OrTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<AndTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<UnderTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr, &filter->IncludeSelf);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<RemoveChildrenTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr, &filter->IncludeSelf);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<LevelRangeTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr, nullptr, &filter->MinLevel, &filter->MaxLevel);
      }

      TreeFilterListItem* CreateFilterPanel(const shared_ptr<NamedTreeFilter>& filter, DeleteCallbackFunction delFunc, EditCallbackFunction editFunc)
      {
         return new TreeFilterListItem(filter, delFunc, editFunc, nullptr);
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Filter panel creation.

   TreeFilterListItem* TreeFilterListItem::Create(
      const TreeFilterPtr& filter,
      DeleteCallbackFunction delFunc,
      EditCallbackFunction editFunc)
   {
      #define CALL_CONVERTER(a) if (auto ptr = dynamic_pointer_cast<a>(filter)) { return CreateFilterPanel(ptr, delFunc, editFunc); }

      CALL_CONVERTER(AcceptTreeFilter)
         CALL_CONVERTER(StopTreeFilter)
         CALL_CONVERTER(UntilTreeFilter)
         CALL_CONVERTER(ContainsTreeFilter)
         CALL_CONVERTER(RegexTreeFilter)
         CALL_CONVERTER(NotTreeFilter)
         CALL_CONVERTER(IfSubTreeTreeFilter)
         CALL_CONVERTER(IfSiblingTreeFilter)
         CALL_CONVERTER(CountChildrenTreeFilter)
         CALL_CONVERTER(CountSiblingsTreeFilter)
         CALL_CONVERTER(OrTreeFilter)
         CALL_CONVERTER(AndTreeFilter)
         CALL_CONVERTER(UnderTreeFilter)
         CALL_CONVERTER(RemoveChildrenTreeFilter)
         CALL_CONVERTER(LevelRangeTreeFilter)
         CALL_CONVERTER(NamedTreeFilter)

         #undef CALL_CONVERTER

         return nullptr;
   }

   TreeFilterListItem::TreeFilterListItem(
      const shared_ptr<TreeFilter>& filter,
      DeleteCallbackFunction delFunc, EditCallbackFunction editFunc,
      wstring* textContent, bool* includeSelf, size_t* count, size_t* count2)
      : Filter(filter)
   {
      const bool active = (delFunc != nullptr);

      setToolTip(QString::fromStdWString(filter->GetDescription()));
      setBackgroundRole(QPalette::ColorRole::Base);
      setAutoFillBackground(true);
      setMouseTracking(true);
      setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

      _defaultBackground = palette();
      _highBackground = palette();
      _highBackground.setColor(QPalette::ColorRole::Base, _highBackground.color(QPalette::Highlight).lighter(210));

      auto container_layout = new QVBoxLayout(this);
      container_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
      container_layout->setMargin(4);
      setLayout(container_layout);

      auto name_layout = new QHBoxLayout;
      name_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
      name_layout->setMargin(0);
      container_layout->addLayout(name_layout);

      auto nameLabel = new QLabel(QString::fromStdWString(filter->GetShortName()));
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

      if (editFunc)
      {
         auto editButton = new QPushButton;
         editButton->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_EDIT)));
         editButton->setToolTip(QString::fromWCharArray(L::t(L"Edit this filter")));
         editButton->setFlat(true);
         editButton->setMaximumSize(QSize(16, 16));
         name_layout->addWidget(editButton);

         editButton->connect(editButton, &QPushButton::clicked, [filter, self = this, editFunc]()
         {
            editFunc(self);
         });
      }

      if (delFunc)
      {
         auto deleteButton = new QPushButton;
         deleteButton->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_DELETE)));
         deleteButton->setToolTip(QString::fromWCharArray(L::t(L"Delete this filter")));
         deleteButton->setFlat(true);
         deleteButton->setMaximumSize(QSize(16, 16));
         name_layout->addWidget(deleteButton);

         deleteButton->connect(deleteButton, &QPushButton::clicked, [filter, self = this, delFunc]()
         {
            delFunc(self);
         });
      }

      if (includeSelf)
      {
         auto includeBox = new QCheckBox(QString::fromWCharArray(L::t(L"Include self")));
         includeBox->setChecked(active);
         container_layout->addWidget(includeBox);

         includeBox->connect(includeBox, &QCheckBox::stateChanged, [includeSelf, includeBox](int)
         {
            *includeSelf = includeBox->isChecked();
         });
      }

      if (auto combine = dynamic_pointer_cast<CombineTreeFilter>(filter))
      {
         SubList = new TreeFilterListWidget(delFunc, editFunc, {}, false);
         SubList->setAcceptDrops(true);
         container_layout->addWidget(SubList);
      }
   }

   TreeFilterListItem* TreeFilterListItem::Clone() const
   {
      return Clone(nullptr, nullptr);
   }

   TreeFilterListItem* TreeFilterListItem::Clone(DeleteCallbackFunction delFunc, EditCallbackFunction editFunc) const
   {
      return Create(Filter->Clone(), delFunc, editFunc);
   }

   void TreeFilterListItem::enterEvent(QEvent* event)
   {
      QWidgetListItem::enterEvent(event);
      HighlightBackground(true);
   }

   void TreeFilterListItem::leaveEvent(QEvent* event)
   {
      QWidgetListItem::leaveEvent(event);
      HighlightBackground(false);
   }

   void TreeFilterListItem::mouseMoveEvent(QMouseEvent* event)
   {
      QWidgetListItem::mouseMoveEvent(event);
   }

   static vector<TreeFilterListItem*> items;

   void TreeFilterListItem::HighlightBackground(bool high)
   {
      if (high)
      {
         if (items.size() > 0)
         {
            items.back()->setPalette(_defaultBackground);
            items.back()->update();
         }
         items.push_back(this);
         setPalette(_highBackground);
      }
      else
      {
         setPalette(_defaultBackground);
         auto pos = std::find(items.begin(), items.end(), this);
         if (pos != items.end())
            items.erase(pos);
      }
      update();
   }

}

