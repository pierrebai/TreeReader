#pragma once

#include "dak/tree_reader/tree_filter.h"

#include <string>
#include <map>
#include <filesystem>

namespace dak::tree_reader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // filter with a name that encapsulate another filter.
   //
   // note: this is not a delegate filter because we don't want
   //       its sub-filter visible to other code. That's the point
   //       of named filter: to hide complexity.

   struct named_filters;

   struct named_tree_filter : tree_filter
   {
      tree_filter_ptr filter;
      std::wstring name;

      named_tree_filter() = default;

      result is_kept(const text_tree& tree, const text_tree::node& node, size_t level) override;
      std::wstring get_short_name() const override;
      std::wstring get_description() const override;
      tree_filter_ptr clone() const override;

   private:
      named_tree_filter(const std::wstring& a_name) : name(a_name) { }
      named_tree_filter(const tree_filter_ptr& a_filter, const std::wstring& a_name) : filter(a_filter), name(a_name) { }

      friend struct named_filters;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Manages a dictionary of named filters.

   typedef std::shared_ptr<named_tree_filter> named_filter_ptr;

   struct named_filters
   {
      // add and remove named filters. add creates a new named filter from
      // a name and the filter to encapsulate.

      named_filter_ptr add(const std::wstring& name, const tree_filter_ptr& filter);
      bool remove(const std::wstring& name);

      // Retrieve one or all named filters.

      named_filter_ptr get(const std::wstring& name) const;
      tree_filter_ptr get_definition(const std::wstring& name) const;
      const std::map<std::wstring, named_filter_ptr>& all() const { return _filters; }

      // merge a set of named filters with another.

      void merge(const named_filters& other);
      
   private:
      std::map<std::wstring, named_filter_ptr> _filters;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Saved and load the named filters into a file.

   void save_named_filters(const std::filesystem::path& path, const named_filters& filters);
   void save_named_filters(std::wostream& stream, const named_filters& filters);

   named_filters load_named_filters(const std::filesystem::path& path);
   named_filters load_named_filters(std::wistream& stream);

   ////////////////////////////////////////////////////////////////////////////
   //
   // Fill the named filters found in the given tree of filters.
   //
   // The named filters in the tree of filters are looked-up in the given
   // dictionary of named filters.
   //
   // This is used to fix a tree of filters after converting from a text format
   // for example. Since named filters are opaque, they were not saved in the
   // text format. They need to be provided after converting from text. This
   // is what this function does.

   void update_named_filters(const tree_filter_ptr& filter, const named_filters& named);
}

