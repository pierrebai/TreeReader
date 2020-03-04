#include "TreeFilterWidget.h"
#include "QtUtilities.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qcheckbox.h>
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
      using DeleteCallbackFunction = TreeFilterWidget::DeleteCallbackFunction;

      namespace L
      {
         inline const wchar_t* t(const wchar_t* text)
         {
            return text;
         }
      }
   }


   /////////////////////////////////////////////////////////////////////////
   //
   // Filter panel creation helpers.

   TreeFilterWidget::TreeFilterWidget(
      const shared_ptr<TreeFilter>& filter,
      DeleteCallbackFunction delFunc,
      const wchar_t* textContent,
      const bool* includeSelf,
      const size_t* count,
      const size_t* count2)
   {
      setToolTip(QString::fromStdWString(filter->GetDescription()));
      setBackgroundRole(QPalette::ColorRole::Base);

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

         deleteButton->connect(deleteButton, &QPushButton::pressed, [filter, self=this, delFunc]()
         {
            delFunc(filter, self);
         });
      }

      if (includeSelf)
      {
         auto includeBox = new QCheckBox(QString::fromWCharArray(L::t(L"Include self")));
         container_layout->addWidget(includeBox);
      }
   }
}

