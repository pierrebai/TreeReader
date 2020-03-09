#pragma once

#include "OptionsDialog.h"

#include <QtWidgets/qdialogbuttonbox.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>

namespace TreeReaderApp
{
   namespace
   {
      namespace L
      {
         inline const wchar_t* t(const wchar_t* text)
         {
            return text;
         }
      }
   }

   // Create the options dialog.
   OptionsDialog::OptionsDialog(CommandsOptions& options, QWidget* parent)
   : QDialog(parent), _options(options)
   {
      BuildUI();
      FillUI();
      ConnectUI();
   }

   void OptionsDialog::accept()
   {
      SaveUI();
   }

   // Create the UI elements.
   void OptionsDialog::BuildUI()
   {
      auto layout = new QVBoxLayout;
      setLayout(layout);

      auto form = new QWidget;
      auto formLayout = new QFormLayout;
      form->setLayout(formLayout);
      layout->addWidget(form);

      _outputLineIndentEdit = new QLineEdit;
      formLayout->addRow(QString::fromWCharArray(L::t(L"Output indentation")), _outputLineIndentEdit);

      _inputIndentEdit = new QLineEdit;
      formLayout->addRow(QString::fromWCharArray(L::t(L"Input indentation")), _inputIndentEdit);

      _inputFilterEdit = new QLineEdit;
      formLayout->addRow(QString::fromWCharArray(L::t(L"Input line filter")), _inputFilterEdit);

      _tabSizeEdit = new QLineEdit;
      formLayout->addRow(QString::fromWCharArray(L::t(L"Tab size")), _tabSizeEdit);

      _buttons = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel);
      layout->addWidget(_buttons);
   }

   // Connect the signals of the UI elements.
   void OptionsDialog::ConnectUI()
   {
      connect(_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
      connect(_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
   }

   // Fill the UI with the intial data.
   void OptionsDialog::FillUI()
   {
      _outputLineIndentEdit->setText(QString::fromStdWString(_options.OutputLineIndent));
      _inputIndentEdit->setText(QString::fromStdWString(_options.ReadOptions.InputIndent));
      _inputFilterEdit->setText(QString::fromStdWString(_options.ReadOptions.InputFilter));
      _tabSizeEdit->setText(QString().setNum(_options.ReadOptions.TabSize));
   }

   // Fill the data from the UI.
   void OptionsDialog::SaveUI()
   {
      _options.OutputLineIndent = _outputLineIndentEdit->text().toStdWString();
      _options.ReadOptions.InputIndent = _inputIndentEdit->text().toStdWString();
      _options.ReadOptions.InputFilter = _inputFilterEdit->text().toStdWString();
      _options.ReadOptions.TabSize = _tabSizeEdit->text().toUInt();
   }
}

// vim: sw=3 : sts=3 : et : sta : 
