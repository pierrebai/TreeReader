#pragma once

#include "TreeFilter.h"

#include <string>
#include <map>
#include <filesystem>

namespace TreeReader
{
   // Manages a dictionary of named filters.

   struct NamedFilters
   {
      std::map<std::wstring, TreeFilterPtr> Filters;

      TreeFilterPtr Get(const std::wstring& name) const;
   };

   // Saved and load the named filters into a file.

   void WriteNamedFilters(const std::filesystem::path& path, const NamedFilters& filters);
   void WriteNamedFilters(std::wostream& stream, const NamedFilters& filters);

   NamedFilters ReadNamedFilters(const std::filesystem::path& path);
   NamedFilters ReadNamedFilters(std::wistream& stream);

   void UpdateNamedFilters(const TreeFilterPtr& filter, const NamedFilters& named);
}

