#pragma once

#include "OptionsDialog.h"

#include <QtWidgets/qdialogbuttonbox.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>

namespace dak::tree_reader::app
{
   // Create the options dialog.
   OptionsDialog::OptionsDialog(commands_options& options, QWidget* parent)
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
      formLayout->addRow(tr("Output indentation"), _outputLineIndentEdit);

      _inputIndentEdit = new QLineEdit;
      formLayout->addRow(tr("Input indentation"), _inputIndentEdit);

      _inputFilterEdit = new QLineEdit;
      formLayout->addRow(tr("Input line filter"), _inputFilterEdit);

      _tabSizeEdit = new QLineEdit;
      formLayout->addRow(tr("Tab size"), _tabSizeEdit);

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
      _outputLineIndentEdit->setText(QString::fromStdWString(_options.output_line_indent));
      _inputIndentEdit->setText(QString::fromStdWString(_options.read_options.input_indent));
      _inputFilterEdit->setText(QString::fromStdWString(_options.read_options.input_filter));
      _tabSizeEdit->setText(QString().setNum(_options.read_options.tab_size));
   }

   // Fill the data from the UI.
   void OptionsDialog::SaveUI()
   {
      _options.output_line_indent = _outputLineIndentEdit->text().toStdWString();
      _options.read_options.input_indent = _inputIndentEdit->text().toStdWString();
      _options.read_options.input_filter = _inputFilterEdit->text().toStdWString();
      _options.read_options.tab_size = _tabSizeEdit->text().toUInt();
   }
}

// vim: sw=3 : sts=3 : et : sta : 
