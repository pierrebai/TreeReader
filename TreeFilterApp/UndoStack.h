#pragma once

#include <any>
#include <vector>
#include <functional>

namespace TreeReaderApp
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Data kept in an undo Transaction.

   class UndoData
   {
   public:
      std::any Data;

      // Remove non-essential data that can be recreated.
      // Called during commit to put the data to sleep.
      std::function<void(std::any&)> Deaden;

      // Recreate the non-essential data and emplace the data in the application.
      // Called during undo or redo to awaken the data.
      std::function<void(const std::any&)> Awaken;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // An object tracking data changing Transactions and undo / redo stack.
   //
   // You initially commit the data that you want to be able to undo back
   // to a Transaction object.
   //
   // ***  You cannot undo if the stack is empty, so don't forget  ***
   // ***  that initial commit!                                    ***
   //
   // Call commit with a Transaction filled with the new data in order
   // to commit that data to the undo stack.
   //
   // The undo function awakens the data that was saved before
   // that last commit. If there are not commit, nothing happens.
   //
   // The redo function awakens the data that was saved. It does nothing
   // if the top the stack is reached.

   class UndoStack
   {
   public:
      //
      typedef UndoData Transaction;
      typedef std::vector<Transaction> Transactions;

      // Create an empty undo stack.
      UndoStack();

      // Clear the undo stack.
      void Clear();

      // Commit the given modified data to the undo stack.
      // Deaden the Transaction data.
      void Commit(const Transaction& tr);

      // Undo awakens the previous Transaction data. (The one before the last commit.)
      // Does nothing if at the start of the undo stack.
      void Undo();

      // Redo awakens the next Transaction data that was commited.
      // Does nothing if at the end of the undo stack.
      void Redo();

      // Verify if there is anything to undo.
      bool HasUndo() const { return _top != _undos.begin(); }

      // Verify if there is anything to redo.
      bool HasRedo() const { return _top != _undos.end() && _top != _undos.end() - 1; }

      // Return the current full contents of the undo stack.
      const Transactions& Contents() const { return _undos; }

   private:
      // Deaden the current top Transaction data.
      void DeadenTop();

      // Awaken the current top Transaction data.
      void AwakenTop() const;

      Transactions _undos;
      Transactions::iterator _top;
   };
}

// vim: sw=3 : sts=3 : et : sta : 

