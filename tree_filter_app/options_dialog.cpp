#pragma once

#include "options_dialog.h"

#include <QtWidgets/qdialogbuttonbox.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>

namespace dak::tree_reader::app
{
   // Create the options dialog.
   options_dialog::options_dialog(commands_options_t& options, QWidget* parent)
   : QDialog(parent), _options(options)
   {
      build_ui();
      fill_ui();
      connect_ui();
   }

   void options_dialog::accept()
   {
      save_ui();
   }

   // Create the UI elements.
   void options_dialog::build_ui()
   {
      auto layout = new QVBoxLayout;
      setLayout(layout);

      auto form = new QWidget;
      auto form_layout = new QFormLayout;
      form->setLayout(form_layout);
      layout->addWidget(form);

      _output_line_indent_edit = new QLineEdit;
      form_layout->addRow(tr("Output indentation"), _output_line_indent_edit);

      _input_indent_edit = new QLineEdit;
      form_layout->addRow(tr("Input indentation"), _input_indent_edit);

      _input_filter_edit = new QLineEdit;
      form_layout->addRow(tr("Input line filter"), _input_filter_edit);

      _tab_size_edit = new QLineEdit;
      form_layout->addRow(tr("Tab size"), _tab_size_edit);

      _buttons = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel);
      layout->addWidget(_buttons);
   }

   // Connect the signals of the UI elements.
   void options_dialog::connect_ui()
   {
      connect(_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
      connect(_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
   }

   // Fill the UI with the intial data.
   void options_dialog::fill_ui()
   {
      _output_line_indent_edit->setText(QString::fromStdWString(_options.output_line_indent));
      _input_indent_edit->setText(QString::fromStdWString(_options.read_options.input_indent));
      _input_filter_edit->setText(QString::fromStdWString(_options.read_options.input_filter));
      _tab_size_edit->setText(QString().setNum(_options.read_options.tab_size));
   }

   // Fill the data from the UI.
   void options_dialog::save_ui()
   {
      _options.output_line_indent = _output_line_indent_edit->text().toStdWString();
      _options.read_options.input_indent = _input_indent_edit->text().toStdWString();
      _options.read_options.input_filter = _input_filter_edit->text().toStdWString();
      _options.read_options.tab_size = _tab_size_edit->text().toUInt();
   }
}

// vim: sw=3 : sts=3 : et : sta : 
