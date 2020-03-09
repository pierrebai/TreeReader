#pragma once

#include "TreeFilterCommands.h"

#include <QtWidgets/qdialog.h>

class QLineEdit;
class QDialogButtonBox;

namespace TreeReaderApp
{
   using CommandsOptions = TreeReader::CommandsOptions;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Options dialog.

   class OptionsDialog : public QDialog
   {
   public:
      // Create the options dialog.
      OptionsDialog(CommandsOptions& options, QWidget* parent = nullptr);

      void accept() override;

   protected:
      // Create the UI elements.
      void BuildUI();

      // Connect the signals of the UI elements.
      void ConnectUI();

      // Fill the UI with the intial data.
      void FillUI();

      // Fill the data from the UI.
      void SaveUI();

      // Data.
      CommandsOptions& _options;

      QLineEdit* _outputLineIndentEdit = nullptr;
      QLineEdit* _inputIndentEdit = nullptr;
      QLineEdit* _inputFilterEdit = nullptr;
      QLineEdit* _tabSizeEdit = nullptr;
      QDialogButtonBox* _buttons = nullptr;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
