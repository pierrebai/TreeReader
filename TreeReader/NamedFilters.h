#pragma once

#include "TreeFilter.h"

#include <string>
#include <map>
#include <filesystem>

namespace TreeReader
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Manages a dictionary of named filters.

   typedef std::shared_ptr<NamedTreeFilter> NamedFilterPtr;

   struct NamedFilters
   {
      const std::map<std::wstring, NamedFilterPtr>& All() const { return _filters; }
      void Merge(const NamedFilters& other);
      NamedFilterPtr Add(const std::wstring& name, const TreeFilterPtr& filter);
      NamedFilterPtr Get(const std::wstring& name) const;
      TreeFilterPtr GetDefinition(const std::wstring& name) const;
      bool Remove(const std::wstring& name);

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
   // The named are matched with the filters found in the given named filters.
   // This is used to fix a tree of filters after converting from a text format
   // for example.

   void UpdateNamedFilters(const TreeFilterPtr& filter, const NamedFilters& named);
}

