#include "TreeFilterPanel.h"
#include "QtUtilities.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qpushbutton.h>

#include <QtGui/qvalidator.h>

#include "resource.h"

namespace TreeReaderApp
{
   using namespace TreeReader;
   using namespace std;

   namespace
   {
      namespace L
      {
         inline const wchar_t* t(const wchar_t* text)
         {
            return text;
         }
      }

      QWidget* CreateFilterPanel(
         const shared_ptr<TreeFilter>& filter,
         DeleteCallbackFunction delFunc,
         const wchar_t* textContent = nullptr,
         const bool* includeSelf = nullptr,
         const size_t* count = nullptr,
         const size_t* count2 = nullptr)
      {
         auto container = new QWidget;
         container->setToolTip(QString::fromStdWString(filter->GetDescription()));
         container->setBackgroundRole(QPalette::ColorRole::Base);

         auto container_layout = new QVBoxLayout(container);
         container_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
         container_layout->setMargin(4);
         container->setLayout(container_layout);

         auto name_layout = new QHBoxLayout;
         name_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
         name_layout->setMargin(0);
         container_layout->addLayout(name_layout);

         auto nameLabel = new QLabel(QString::fromStdWString(filter->GetShortName()));
         name_layout->addWidget(nameLabel);

         if (textContent)
         {
            auto textEdit = new QLineEdit(QString::fromWCharArray(textContent));
            textEdit->setMaximumWidth(80);
            name_layout->addWidget(textEdit);
         }

         if (count)
         {
            auto numberEdit = new QLineEdit(QString().setNum(*count));
            numberEdit->setMaximumWidth(count2 ? 40 : 80);
            numberEdit->setMaxLength(3);
            numberEdit->setValidator(new QIntValidator);
            name_layout->addWidget(numberEdit);
         }

         if (count2)
         {
            auto numberEdit = new QLineEdit(QString().setNum(*count2));
            numberEdit->setMaximumWidth(count ? 40 : 80);
            numberEdit->setMaxLength(3);
            numberEdit->setValidator(new QIntValidator);
            name_layout->addWidget(numberEdit);
         }

         if (delFunc)
         {
            auto deleteButton = new QPushButton;
            deleteButton->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_DELETE)));
            deleteButton->setFlat(true);
            deleteButton->setMaximumSize(QSize(16, 16));
            name_layout->addWidget(deleteButton);

            deleteButton->connect(deleteButton, &QPushButton::pressed, [filter, container, delFunc]()
            {
               delFunc(filter, container);
            });
         }

         if (includeSelf)
         {
            auto includeBox = new QCheckBox(QString::fromWCharArray(L::t(L"Include self")));
            container_layout->addWidget(includeBox);
         }

         return container;
      }

      QWidget* CreateFilterPanel(const shared_ptr<AcceptTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<StopTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<UntilTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<ContainsTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, filter->Contained.c_str());
      }

      QWidget* CreateFilterPanel(const shared_ptr<RegexTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, filter->RegexTextForm.c_str());
      }

      QWidget* CreateFilterPanel(const shared_ptr<NotTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<IfSubTreeTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<IfSiblingTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<CountChildrenTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr, &filter->IncludeSelf, &filter->Count);
      }

      QWidget* CreateFilterPanel(const shared_ptr<CountSiblingsTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr, &filter->IncludeSelf, &filter->Count);
      }

      QWidget* CreateFilterPanel(const shared_ptr<OrTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<AndTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }

      QWidget* CreateFilterPanel(const shared_ptr<UnderTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr, &filter->IncludeSelf);
      }

      QWidget* CreateFilterPanel(const shared_ptr<RemoveChildrenTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr, &filter->IncludeSelf);
      }

      QWidget* CreateFilterPanel(const shared_ptr<LevelRangeTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr, nullptr, &filter->MinLevel, &filter->MaxLevel);
      }

      QWidget* CreateFilterPanel(const shared_ptr<NamedTreeFilter>& filter, DeleteCallbackFunction delFunc)
      {
         return CreateFilterPanel(filter, delFunc, nullptr);
      }
   }

   QWidget* ConvertFilterToPanel(const TreeFilterPtr& filter, DeleteCallbackFunction delFunc)
   {
      #define CALL_CONVERTER(a) if (auto ptr = dynamic_pointer_cast<a>(filter)) { return CreateFilterPanel(ptr, delFunc); }

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

   QScrollArea* CreateTreeFilterList()
   {
      auto availLayout = new QVBoxLayout;
      availLayout->setSizeConstraint(QLayout::SetMinimumSize);
      availLayout->setMargin(2);

      auto availWidget = new QWidget;
      availWidget->setBackgroundRole(QPalette::ColorRole::Base);
      availWidget->setLayout(availLayout);

      auto availList = new QScrollArea;
      availList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      availList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      availList->setWidget(availWidget);
      availList->setWidgetResizable(true);
      availList->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

      return availList;
   }

   void ClearTreeFilterList(QScrollArea* list)
   {
      if (!list)
         return;

      auto availWidget = list->widget();
      if (!availWidget)
         return;

      for (auto child : list->children())
         if (!dynamic_cast<QLayout*>(child))
            delete child;
   }

   void AddTreeFilterPanel(QScrollArea* list, const TreeFilterPtr& filter, DeleteCallbackFunction delFunc)
   {
      if (!list)
         return;

      auto availWidget = list->widget();
      if (!availWidget)
         return;

      auto layout = availWidget->layout();
      if (!layout)
         return;

      auto widget = ConvertFilterToPanel(filter, delFunc);
      if (!widget)
         return;

      layout->addWidget(widget);

      availWidget->setMinimumWidth(max(availWidget->minimumWidth(), widget->sizeHint().width()));
      list->setMinimumWidth(max(list->minimumWidth(), widget->sizeHint().width()));
   }
}
