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

   class FiltersEditor : public QWidget
   {
   public:
      // What is being edited.
      typedef std::vector<TreeFilterPtr> Filters;

      // Callback when the selection changed.
      typedef std::function<void(const Filters&)> SelectionChangedCallback;
      SelectionChangedCallback SelectionChanged;

      // Callback when the edited filters are changed.
      typedef std::function<void(const Filters&)> FiltersChangedCallback;
      FiltersChangedCallback FiltersChanged;

      // Callback when a new filter should be added.
      typedef std::function<void()> NewFilterRequestedCallback;
      NewFilterRequestedCallback NewFilterRequested;

      // Create a filter editor with the given parent widget.
      FiltersEditor(QWidget* parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon);

      // Set the Filters being edited.
      void SetEdited(const Filters& edited);
      const Filters& GetEdited() const;

      // Update the list UI to represent changes made to filters.
      void UpdateListContent();

      // Retrieve the selected Filters.
      Filters GetSelectedFilters() const;

   protected:
      std::unique_ptr<FiltersEditorUI> _ui;
   };
}

// vim: sw=3 : sts=3 : et : sta : 
