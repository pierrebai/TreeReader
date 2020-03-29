#pragma once

#include <any>
#include <vector>
#include <functional>

namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Data kept in an undo transaction.

   class undo_data
   {
   public:
      // The data.
      std::any data;

      // Remove non-essential data that can be recreated.
      // Called during commit to put the data to sleep.
      std::function<void(std::any&)> deaden;

      // Recreate the non-essential data and emplace the data in the application.
      // Called during undo or redo to awaken the data.
      std::function<void(const std::any&)> awaken;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // An object tracking data changing transactions and undo / redo stack.
   //
   // You initially commit the data that you want to be able to undo back
   // to a transaction object.
   //
   // ***  You cannot undo if the stack is empty, so don't forget  ***
   // ***  that initial commit!                                    ***
   //
   // Call commit with a transaction filled with the new data in order
   // to commit that data to the undo stack.
   //
   // The undo function awakens the data that was saved before
   // that last commit. If there are not commit, nothing happens.
   //
   // The redo function awakens the data that was saved. It does nothing
   // if the top the stack is reached.

   class undo_stack
   {
   public:
      // The undo/redo transactions kept in the stack.
      typedef undo_data transaction;
      typedef std::vector<transaction> transactions;

      // The function called when teh undo stack changed (clear, undo or redo called).
      std::function<void(undo_stack &)> changed;

      // Create an empty undo stack.
      undo_stack();

      // clear the undo stack.
      void clear();

      // commit the given modified data to the undo stack.
      // deaden the transaction data.
      void commit(const transaction& tr);

      // undo awakens the previous transaction data. (The one before the last commit.)
      // Does nothing if at the start of the undo stack.
      void undo();

      // redo awakens the next transaction data that was commited.
      // Does nothing if at the end of the undo stack.
      void redo();

      // Verify if there is anything to undo.
      bool has_undo() const { return _top != _undos.begin(); }

      // Verify if there is anything to redo.
      bool has_redo() const { return _top != _undos.end() && _top != _undos.end() - 1; }

      // Verify if an commit/undo/redo operation is underway.
      bool IsUndoing() const { return _is_undoing; }

      // Return the current full contents of the undo stack.
      const transactions& Contents() const { return _undos; }

   private:
      // deaden the current top transaction data.
      void deaden_top();

      // awaken the current top transaction data.
      void awaken_top() const;

      transactions _undos;
      transactions::iterator _top;
      bool _is_undoing = false;
   };
}

// vim: sw=3 : sts=3 : et : sta : 

