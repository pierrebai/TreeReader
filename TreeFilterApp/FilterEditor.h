#pragma once

#include "TreeFilter.h"

#include <QtWidgets/qwidget>

#include <memory>
#include <functional>

namespace TreeReaderApp
{
   class FiltersEditorUI;
   using TreeFilter = TreeReader::TreeFilter;
   using TreeFilterPtr = TreeReader::TreeFilterPtr;

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order Filters.

   class FilterEditor : public QWidget
   {
   public:
      // Callback when the edited filter is changed.
      typedef std::function<void(const TreeFilterPtr&)> FilterChangedCallback;
      FilterChangedCallback FilterChanged;

      // Create a filter editor with the given parent widget.
      FilterEditor(QWidget* parent);

      // Set the Filters being edited.
      void SetEdited(const TreeFilterPtr& edited);
      TreeFilterPtr GetEdited() const;

      // Update the list UI to represent changes made to filters.
      void UpdateListContent();

   protected:
      std::unique_ptr<FiltersEditorUI> _ui;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
