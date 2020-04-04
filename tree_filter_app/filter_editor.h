#pragma once

#include <QtWidgets/QWidget>

#include "dak/utility/undo_stack.h"

#include <memory>
#include <functional>

namespace dak::utility
{
   class undo_stack;
}

namespace dak::tree_reader
{
   struct tree_filter;
   struct named_filters;
}

namespace dak::tree_reader::app
{
   class filters_editor_ui;
   using tree_filter_ptr = std::shared_ptr<tree_filter>;
   using undo_stack = dak::utility::undo_stack;

   ////////////////////////////////////////////////////////////////////////////
   //
   // A QWidget to select and order filters.

   struct filter_editor : QWidget
   {
      // Callback when the edited filter is changed.
      typedef std::function<void(const tree_filter_ptr&)> filter_changed_callback;
      filter_changed_callback filter_changed;

      // Create a filter editor with the given parent widget.
      filter_editor(const named_filters& known, undo_stack& undoRedo, QWidget* parent);

      // Set the named_filters being edited.
      void set_edited(const tree_filter_ptr& edited, const std::wstring& name, bool forced = false);

      // get the filter being edited.
      tree_filter_ptr get_edited() const;
      std::wstring get_edited_name() const;

   protected:
      std::unique_ptr<filters_editor_ui> _ui;

      Q_OBJECT;

   };
}

// vim: sw=3 : sts=3 : et : sta : 
