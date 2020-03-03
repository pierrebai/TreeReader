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
         const TreeFilter& filter,
         const wchar_t* textContent = nullptr,
         const bool* includeSelf = nullptr,
         const size_t* count = nullptr,
         const size_t* count2 = nullptr)
      {
         auto container = new QWidget;
         container->setToolTip(QString::fromStdWString(filter.GetDescription()));
         container->setBackgroundRole(QPalette::ColorRole::Base);

         auto container_layout = new QVBoxLayout(container);
         container_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
         container_layout->setMargin(4);
         container->setLayout(container_layout);

         auto name_layout = new QHBoxLayout;
         name_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
         name_layout->setMargin(0);
         container_layout->addLayout(name_layout);

         auto nameLabel = new QLabel(QString::fromStdWString(filter.GetShortName()));
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

         auto deleteButton = new QPushButton;
         deleteButton->setIcon(QIcon(CreatePixmapFromResource(IDB_FILTER_DELETE)));
         deleteButton->setFlat(true);
         deleteButton->setMaximumSize(QSize(16, 16));
         name_layout->addWidget(deleteButton);

         if (includeSelf)
         {
            auto includeBox = new QCheckBox(QString::fromWCharArray(L::t(L"Include self")));
            container_layout->addWidget(includeBox);
         }

         return container;
      }

      QWidget* ConvertFilterToPanel(const AcceptTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }

      QWidget* ConvertFilterToPanel(const StopTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }

      QWidget* ConvertFilterToPanel(const UntilTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }

      QWidget* ConvertFilterToPanel(const ContainsTreeFilter& filter)
      {
         return CreateFilterPanel(filter, filter.Contained.c_str());
      }

      QWidget* ConvertFilterToPanel(const RegexTreeFilter& filter)
      {
         return CreateFilterPanel(filter, filter.RegexTextForm.c_str());
      }

      QWidget* ConvertFilterToPanel(const NotTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }

      QWidget* ConvertFilterToPanel(const IfSubTreeTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }

      QWidget* ConvertFilterToPanel(const IfSiblingTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }

      QWidget* ConvertFilterToPanel(const CountChildrenTreeFilter& filter)
      {
         return CreateFilterPanel(filter, nullptr, &filter.IncludeSelf, &filter.Count);
      }

      QWidget* ConvertFilterToPanel(const CountSiblingsTreeFilter& filter)
      {
         return CreateFilterPanel(filter, nullptr, &filter.IncludeSelf, &filter.Count);
      }

      QWidget* ConvertFilterToPanel(const OrTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }

      QWidget* ConvertFilterToPanel(const AndTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }

      QWidget* ConvertFilterToPanel(const UnderTreeFilter& filter)
      {
         return CreateFilterPanel(filter, nullptr, &filter.IncludeSelf);
      }

      QWidget* ConvertFilterToPanel(const RemoveChildrenTreeFilter& filter)
      {
         return CreateFilterPanel(filter, nullptr, &filter.IncludeSelf);
      }

      QWidget* ConvertFilterToPanel(const LevelRangeTreeFilter& filter)
      {
         return CreateFilterPanel(filter, nullptr, nullptr, &filter.MinLevel, &filter.MaxLevel);
      }

      QWidget* ConvertFilterToPanel(const NamedTreeFilter& filter)
      {
         return CreateFilterPanel(filter);
      }
   }

   QWidget* ConvertFilterToPanel(const TreeFilterPtr& filter)
   {
      #define CALL_CONVERTER(a) if (auto ptr = dynamic_pointer_cast<a>(filter)) { return ConvertFilterToPanel(*ptr); }

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

   void AddTreeFilterPanel(QScrollArea* list, const TreeFilterPtr& filter)
   {
      if (!list)
         return;

      auto availWidget = list->widget();
      if (!availWidget)
         return;

      auto layout = availWidget->layout();
      if (!layout)
         return;

      auto widget = ConvertFilterToPanel(filter);
      if (!widget)
         return;

      layout->addWidget(widget);

      availWidget->setMinimumWidth(max(availWidget->minimumWidth(), widget->sizeHint().width()));
      list->setMinimumWidth(max(list->minimumWidth(), widget->sizeHint().width()));
   }
}
