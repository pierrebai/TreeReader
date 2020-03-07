#include "UndoStack.h"

namespace TreeReader
{
   namespace
   {
      struct Undoing
      {
         Undoing(bool& f) : _f(f)
         {
            _f = true;
         }

         ~Undoing()
         {
            _f = false;
         }

      private:
         bool& _f;
      };
   }

   // Create an empty undo stack.
   UndoStack::UndoStack()
   : _top(_undos.end())
   {
   }

   // Clear the undo stack.
   void UndoStack::Clear()
   {
      _undos.clear();
      _top = _undos.end();

      if (Changed)
         Changed(*this);
   }

   // Deaden the current top Transaction data.
   void UndoStack::DeadenTop()
   {
      if (_top->Deaden)
         _top->Deaden(_top->Data);
   }

   // Awaken the current top Transaction data.
   void UndoStack::AwakenTop() const
   {
      if (_top->Awaken)
         _top->Awaken(_top->Data);
   }

   // Commit the given modified data to the undo stack.
   // Deaden the Transaction data.
   void UndoStack::Commit(const Transaction& tr)
   {
      // Refuse to commit during undo/redo/commit.
      if (_isUndoing)
         return;

      // If there were undone Transactions, erase them now that we're commiting a new timeline.
      if (HasRedo())
         _undos.erase(_top + 1, _undos.end());

      Undoing undoing(_isUndoing);

      _undos.emplace_back(tr);
      _top = _undos.end() - 1;
      DeadenTop();

      if (Changed)
         Changed(*this);
   }

   // Undo awakens the previous Transaction data. (The one before the last commit.)
   // Does nothing if at the start of the undo stack.
   void UndoStack::Undo()
   {
      if (!HasUndo())
         return;

      Undoing undoing(_isUndoing);

      --_top;
      AwakenTop();

      if (Changed)
         Changed(*this);
   }

   // Redo awakens the next Transaction data that was commited.
   // Does nothing if at the end of the undo stack.
   void UndoStack::Redo()
   {
      if (!HasRedo())
         return;

      Undoing undoing(_isUndoing);

      ++_top;
      AwakenTop();

      if (Changed)
         Changed(*this);
   }
}

// vim: sw=3 : sts=3 : et : sta : 

