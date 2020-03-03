#include "TreeFilterPanel.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qcheckbox.h>
#include <QtGui/qvalidator.h>

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
         QWidget* container = new QWidget;
         container->setToolTip(QString::fromStdWString(filter.GetDescription()));
         container->setBackgroundRole(QPalette::ColorRole::Base);

         QVBoxLayout* container_layout = new QVBoxLayout(container);
         container_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
         container_layout->setMargin(4);
         container->setLayout(container_layout);

         QHBoxLayout* name_layout = new QHBoxLayout;
         name_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
         name_layout->setMargin(0);
         container_layout->addLayout(name_layout);

         QLabel* nameLabel = new QLabel(QString::fromStdWString(filter.GetShortName()));
         name_layout->addWidget(nameLabel);

         if (textContent)
         {
            QLineEdit* textEdit = new QLineEdit(QString::fromWCharArray(textContent));
            textEdit->setMaximumWidth(80);
            name_layout->addWidget(textEdit);
         }

         if (count)
         {
            QLineEdit* numberEdit = new QLineEdit(QString().setNum(*count));
            numberEdit->setMaximumWidth(count2 ? 40 : 80);
            numberEdit->setMaxLength(3);
            numberEdit->setValidator(new QIntValidator);
            name_layout->addWidget(numberEdit);
         }

         if (count2)
         {
            QLineEdit* numberEdit = new QLineEdit(QString().setNum(*count2));
            numberEdit->setMaximumWidth(count ? 40 : 80);
            numberEdit->setMaxLength(3);
            numberEdit->setValidator(new QIntValidator);
            name_layout->addWidget(numberEdit);
         }

         // TODO: add delete button.

         if (includeSelf)
         {
            QCheckBox* includeBox = new QCheckBox(QString::fromWCharArray(L::t(L"Include self")));
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

   void AddTreeFilterPanel(QLayout* list, const TreeFilterPtr& filter)
   {
      if (!list)
         return;

      QWidget* widget = ConvertFilterToPanel(filter);
      if (!widget)
         return;

      list->addWidget(widget);
   }

}
