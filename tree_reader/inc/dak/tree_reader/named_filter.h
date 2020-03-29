#pragma once

#include "TreeFilter.h"

#include <string>
#include <map>
#include <filesystem>

namespace TreeReader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Filter with a name that encapsulate another filter.
   //
   // Note: this is not a delegate filter because we don't want
   //       its sub-filter visible to other code. That's the point
   //       of named filter: to hide complexity.

   struct NamedFilters;

   struct NamedTreeFilter : TreeFilter
   {
      TreeFilterPtr Filter;
      std::wstring Name;

      NamedTreeFilter() = default;

      Result IsKept(const TextTree& tree, const TextTree::Node& node, size_t level) override;
      std::wstring GetShortName() const override;
      std::wstring GetDescription() const override;
      TreeFilterPtr Clone() const override;

   private:
      NamedTreeFilter(const std::wstring& name) : Name(name) { }
      NamedTreeFilter(const TreeFilterPtr& filter, const std::wstring& name) : Filter(filter), Name(name) { }

      friend struct NamedFilters;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Manages a dictionary of named filters.

   typedef std::shared_ptr<NamedTreeFilter> NamedFilterPtr;

   struct NamedFilters
   {
      // Add and remove named filters. Add creates a new named filter from
      // a name and the filter to encapsulate.

      NamedFilterPtr Add(const std::wstring& name, const TreeFilterPtr& filter);
      bool Remove(const std::wstring& name);

      // Retrieve one or all named filters.

      NamedFilterPtr Get(const std::wstring& name) const;
      TreeFilterPtr GetDefinition(const std::wstring& name) const;
      const std::map<std::wstring, NamedFilterPtr>& All() const { return _filters; }

      // Merge a set of named filters with another.

      void Merge(const NamedFilters& other);
      
   private:
      std::map<std::wstring, NamedFilterPtr> _filters;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Saved and load the named filters into a file.

   void WriteNamedFilters(const std::filesystem::path& path, const NamedFilters& filters);
   void WriteNamedFilters(std::wostream& stream, const NamedFilters& filters);

   NamedFilters ReadNamedFilters(const std::filesystem::path& path);
   NamedFilters ReadNamedFilters(std::wistream& stream);

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

   void UpdateNamedFilters(const TreeFilterPtr& filter, const NamedFilters& named);
}

