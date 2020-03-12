#pragma once

#include "TreeFilter.h"
#include "UndoStack.h"

#include <QtWidgets/qwidget>

#include <memory>
#include <functional>

namespace TreeReaderApp
{
   class FiltersEditorUI;
   using TreeFilter = TreeReader::TreeFilter;
   using TreeFilterPtr = TreeReader::TreeFilterPtr;
   using UndoStack = TreeReader::UndoStack;
   using NamedFilters = TreeReader::NamedFilters;

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   struct FilterEditor : QWidget
   {
      // Callback when the edited filter is changed.
      typedef std::function<void(const TreeFilterPtr&)> FilterChangedCallback;
      FilterChangedCallback FilterChanged;

      // Create a filter editor with the given parent widget.
      FilterEditor(const NamedFilters& known, UndoStack& undoRedo, QWidget* parent);

      // Set the Filters being edited.
      void SetEdited(const TreeFilterPtr& edited, const std::wstring& name, bool forced = false);

      // Get the filter being edited.
      TreeFilterPtr GetEdited() const;
      std::wstring GetEditedName() const;

   protected:
      std::unique_ptr<FiltersEditorUI> _ui;

      Q_OBJECT;

   };
}

// vim: sw=3 : sts=3 : et : sta : 
