#include "dak/tree_reader/named_filters.h"
#include "dak/tree_reader/tree_filter_maker.h"
#include "dak/tree_reader/tree_filter_helpers.h"
#include "dak/utility/text.h"

#include <fstream>
#include <iomanip>

namespace dak::tree_reader
{
   using namespace std;

   named_filter_ptr named_filters::add(const std::wstring& name, const tree_filter_ptr& filter)
   {
      if (name.empty())
         return {};

      named_filter_ptr named(new named_tree_filter(filter, name));
      _filters[name] = named;
      return named;
   }

   bool named_filters::remove(const wstring& name)
   {
      const auto pos = _filters.find(name);
      if (pos == _filters.end())
         return false;

      _filters.erase(pos);
      return true;
   }

   void named_filters::merge(const named_filters& other)
   {
      _filters.insert(other.all().begin(), other.all().end());
   }

   named_filter_ptr named_filters::get(const wstring& name) const
   {
      const auto pos = _filters.find(name);
      if (pos == _filters.end())
         return {};

      return pos->second;
   }

   tree_filter_ptr named_filters::get_definition(const std::wstring& name) const
   {
      auto named = dynamic_pointer_cast<named_tree_filter>(get(name));
      if (!named)
         return {};

      return named->filter;
   }

   void save_named_filters(const filesystem::path& path, const named_filters& filters)
   {
      wofstream stream(path);
      save_named_filters(stream, filters);
   }

   void save_named_filters(wostream& stream, const named_filters& filters)
   {
      for (const auto& [name, filter] : filters.all())
         if (filter)
            stream << quoted(name) << L" : " << quoted(convert_filter_to_text(filter->filter)) << endl;
   }

   named_filters load_named_filters(const filesystem::path& path)
   {
      wifstream stream(path);
      return load_named_filters(stream);
   }

   named_filters load_named_filters(wistream& stream)
   {
      named_filters filters;

      while (stream)
      {
         wstring name;
         wstring filter_text;
         wchar_t column;
         stream >> skipws >> quoted(name) >> skipws >> column >> skipws >> quoted(filter_text);

         tree_filter_ptr filter = convert_text_to_filter(filter_text, filters);

         filters.add(name, filter);
      }

      for (auto& [name, filter] : filters.all())
         update_named_filters(filter, filters);

      return filters;
   }

   void update_named_filters(const tree_filter_ptr& filter, const named_filters& named)
   {
      visit_filters(filter, [&named](const tree_filter_ptr& filter)
      {
         if (auto named_filter = dynamic_pointer_cast<named_tree_filter>(filter))
         {
            auto target_filter = named.get_definition(named_filter->name);
            if (target_filter)
               named_filter->filter = target_filter;
         }
         return true;
      });
   }
}

