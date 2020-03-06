#include "NamedFilters.h"
#include "TreeFilterMaker.h"
#include "TreeFilterHelpers.h"

#include <fstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

   NamedFilterPtr NamedFilters::Add(const std::wstring& name, const TreeFilterPtr& filter)
   {
      if (name.empty())
         return {};

      NamedFilterPtr named(new NamedTreeFilter(filter, name));
      _filters[name] = named;
      return named;
   }

   bool NamedFilters::Remove(const wstring& name)
   {
      const auto pos = _filters.find(name);
      if (pos == _filters.end())
         return false;

      _filters.erase(pos);
      return true;
   }

   void NamedFilters::Merge(const NamedFilters& other)
   {
      _filters.insert(other.All().begin(), other.All().end());
   }

   NamedFilterPtr NamedFilters::Get(const wstring& name) const
   {
      const auto pos = _filters.find(name);
      if (pos == _filters.end())
         return {};

      return pos->second;
   }

   TreeFilterPtr NamedFilters::GetDefinition(const std::wstring& name) const
   {
      auto named = dynamic_pointer_cast<NamedTreeFilter>(Get(name));
      if (!named)
         return {};

      return named->Filter;
   }

   void WriteNamedFilters(const filesystem::path& path, const NamedFilters& filters)
   {
      wofstream stream(path);
      WriteNamedFilters(stream, filters);
   }

   void WriteNamedFilters(wostream& stream, const NamedFilters& filters)
   {
      for (const auto& [name, filter] : filters.All())
         if (filter)
            stream << quoted(name) << L" : " << quoted(ConvertFiltersToText(filter->Filter)) << endl;
   }

   NamedFilters ReadNamedFilters(const filesystem::path& path)
   {
      wifstream stream(path);
      return ReadNamedFilters(stream);
   }

   NamedFilters ReadNamedFilters(wistream& stream)
   {
      NamedFilters filters;

      while (stream)
      {
         wstring name;
         wstring filterText;
         wchar_t column;
         stream >> skipws >> quoted(name) >> skipws >> column >> skipws >> quoted(filterText);

         TreeFilterPtr filter = ConvertTextToFilters(filterText, filters);

         filters.Add(name, filter);
      }

      for (auto& [name, filter] : filters.All())
         UpdateNamedFilters(filter, filters);

      return filters;
   }

   void UpdateNamedFilters(const TreeFilterPtr& filter, const NamedFilters& named)
   {
      VisitFilters(filter, [&named](const TreeFilterPtr& filter)
      {
         if (auto namedFilter = dynamic_pointer_cast<NamedTreeFilter>(filter))
         {
            auto targetFilter = named.GetDefinition(namedFilter->Name);
            if (targetFilter)
               namedFilter->Filter = targetFilter;
         }
         return true;
      });
   }
}

