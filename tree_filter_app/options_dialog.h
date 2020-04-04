#pragma once

#include "dak/tree_reader/global_commands.h"

#include <QtWidgets/qdialog.h>

class QLineEdit;
class QDialogButtonBox;

namespace dak::tree_reader::app
{
   using commands_options = tree_reader::commands_options;

   ////////////////////////////////////////////////////////////////////////////
   //
   // options dialog.

   struct options_dialog : QDialog
   {
      // Create the options dialog.
      options_dialog(commands_options& options, QWidget* parent = nullptr);

      void accept() override;

   protected:
      // Create the UI elements.
      void build_ui();

      // Connect the signals of the UI elements.
      void connect_ui();

      // Fill the UI with the intial data.
      void fill_ui();

      // Fill the data from the UI.
      void save_ui();

      // data.
      commands_options& _options;

      QLineEdit* _output_line_indent_edit = nullptr;
      QLineEdit* _input_indent_edit = nullptr;
      QLineEdit* _input_filter_edit = nullptr;
      QLineEdit* _tab_size_edit = nullptr;
      QDialogButtonBox* _buttons = nullptr;

      Q_OBJECT;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
