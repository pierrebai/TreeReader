#include "NamedFilters.h"
#include "TreeFilterMaker.h"

#include <fstream>
#include <iomanip>

namespace TreeReader
{
   using namespace std;

   TreeFilterPtr NamedFilters::Get(const wstring& name) const
   {
      const auto pos = Filters.find(name);
      if (pos == Filters.end())
         return {};

      return pos->second;
   }

   void WriteNamedFilters(const filesystem::path& path, const NamedFilters& filters)
   {
      wofstream stream(path);
      WriteNamedFilters(stream, filters);
   }

   void WriteNamedFilters(wostream& stream, const NamedFilters& filters)
   {
      for (const auto& [name, filter] : filters.Filters)
         if (filter)
            stream << quoted(name) << L" : " << quoted(ConvertFiltersToText(filter)) << endl;
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
         if (name.empty())
            continue;

         TreeFilterPtr filter = ConvertTextToFilters(filterText, filters);
         if (!filter)
            continue;

         filters.Filters[name] = filter;
      }

      for (auto& [name, filter] : filters.Filters)
         UpdateNamedFilters(filter, filters);

      return filters;
   }

   void UpdateNamedFilters(const TreeFilterPtr& filter, const NamedFilters& named)
   {
      if (auto namedFilter = dynamic_pointer_cast<NamedTreeFilter>(filter))
      {
         namedFilter->Filter = named.Get(namedFilter->Name);
      }
      else if (auto delegate = dynamic_pointer_cast<DelegateTreeFilter>(filter))
      {
         UpdateNamedFilters(delegate->Filter, named);
      }
      else if (auto combined = dynamic_pointer_cast<CombineTreeFilter>(filter))
      {
         for (auto& child : combined->Filters)
         {
            UpdateNamedFilters(child, named);
         }
      }
   }
}

