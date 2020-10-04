#include "dak/tree_reader/tree_filter_maker.h"

#include <sstream>
#include <iomanip>

namespace dak::tree_reader
{
   using namespace std;

   namespace V1
   {
      wstring convert_filter_to_text(const tree_filter_ptr_t& filter, size_t indent);

      wstring convert_filter_to_text(const accept_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"accept [ ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const stop_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"stop [ ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const stop_when_kept_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"stop-when-kept [ " << convert_filter_to_text(filter.sub_filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const until_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"until [ " << convert_filter_to_text(filter.sub_filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const contains_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"contains [ " << quoted(filter.contained) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const unique_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"unique [ ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const regex_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"regex [ " << quoted(filter.regex_text) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const not_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"not [ " << convert_filter_to_text(filter.sub_filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const if_subtree_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"if-sub [ " << convert_filter_to_text(filter.sub_filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const if_sibling_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"if-sib [ " << convert_filter_to_text(filter.sub_filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(vector<tree_filter_ptr_t> filters, size_t indent)
      {
         wostringstream sstream;

         auto begin = filters.begin();
         const auto end = filters.end();

         if (begin != end)
         {
            sstream << convert_filter_to_text(*begin, indent);
            ++begin;
         }

         while (begin != end)
         {
            sstream << L", ";
            sstream << convert_filter_to_text(*begin, indent);
            ++begin;
         }

         return sstream.str();
      }

      wstring convert_filter_to_text(const or_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"or [ " << convert_filter_to_text(filter.filters, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const and_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"and [ " << convert_filter_to_text(filter.filters, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const under_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"under [ " << boolalpha << filter.include_self << L", " << convert_filter_to_text(filter.sub_filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const remove_children_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"no-child [ " << boolalpha << filter.include_self << L", " << convert_filter_to_text(filter.sub_filter, indent + 1) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const level_range_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"range [ " << filter.min_level << L", " << filter.max_level << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const named_tree_filter_t& filter, size_t indent)
      {
         wostringstream sstream;
         sstream << L"named [ " << quoted(filter.name) << L" ]";
         return sstream.str();
      }

      wstring convert_filter_to_text(const tree_filter_t& filter, size_t indent)
      {
         wstring indent_text = wstring(L"\n") + wstring(indent, L' ');
         #define CALL_CONVERTER(a) if (const a* ptr = dynamic_cast<const a *>(&filter)) { return indent_text + convert_filter_to_text(*ptr, indent); }

         CALL_CONVERTER(accept_tree_filter_t)
         CALL_CONVERTER(stop_tree_filter_t)
         CALL_CONVERTER(stop_when_kept_tree_filter_t)
         CALL_CONVERTER(until_tree_filter_t)
         CALL_CONVERTER(contains_tree_filter_t)
         CALL_CONVERTER(unique_tree_filter_t)
         CALL_CONVERTER(regex_tree_filter_t)
         CALL_CONVERTER(not_tree_filter_t)
         CALL_CONVERTER(if_subtree_tree_filter_t)
         CALL_CONVERTER(if_sibling_tree_filter_t)
         CALL_CONVERTER(or_tree_filter_t)
         CALL_CONVERTER(and_tree_filter_t)
         CALL_CONVERTER(under_tree_filter_t)
         CALL_CONVERTER(remove_children_tree_filter_t)
         CALL_CONVERTER(level_range_tree_filter_t)
         CALL_CONVERTER(named_tree_filter_t)

         #undef CALL_CONVERTER

         return {};
      }

      wstring convert_filter_to_text(const tree_filter_ptr_t& filter, size_t indent)
      {
         if (!filter)
            return {};

         return convert_filter_to_text(*filter, indent);
      }

      wstring convert_filter_to_text(const tree_filter_ptr_t& filter)
      {
         wostringstream sstream;
         sstream << L"V1: " << convert_filter_to_text(filter, 0);
         return sstream.str();
      }

      template <class T>
      tree_filter_ptr_t convert_text_to_filter(wistringstream& sstream);

      tree_filter_ptr_t convert_text_to_filter(wistringstream& sstream);

      void eat_closing_brace(wistringstream& sstream)
      {
         // Eat the closing ']'.
         wchar_t brace;
         sstream >> skipws >> brace;
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<accept_tree_filter_t>(wistringstream& sstream)
      {
         eat_closing_brace(sstream);
         return accept();
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<stop_tree_filter_t>(wistringstream& sstream)
      {
         eat_closing_brace(sstream);
         return stop();
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<stop_when_kept_tree_filter_t>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         eat_closing_brace(sstream);

         return stop_when_kept(filter);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<until_tree_filter_t>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         eat_closing_brace(sstream);

         return until(filter);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<contains_tree_filter_t>(wistringstream& sstream)
      {
         wstring contained;
         sstream >> skipws >> quoted(contained);

         eat_closing_brace(sstream);

         return contains(contained);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<unique_tree_filter_t>(wistringstream& sstream)
      {
         eat_closing_brace(sstream);
         return unique();
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<regex_tree_filter_t>(wistringstream& sstream)
      {
         wstring regex;
         sstream >> skipws >> quoted(regex);

         eat_closing_brace(sstream);

         return dak::tree_reader::regex(regex);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<not_tree_filter_t>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         eat_closing_brace(sstream);

         return not(filter);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<if_subtree_tree_filter_t>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         eat_closing_brace(sstream);

         return if_subtree(filter);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<if_sibling_tree_filter_t>(wistringstream& sstream)
      {
         auto filter = convert_text_to_filter(sstream);

         eat_closing_brace(sstream);

         return if_sibling(filter);
      }

      vector<tree_filter_ptr_t> convert_text_to_multiple_filters(wistringstream& sstream)
      {
         vector<tree_filter_ptr_t> filters;

         while (sstream)
         {
            wchar_t comma;
            sstream >> skipws >> comma;
            if (comma != L',')
               sstream.putback(comma);
            if (comma == L']')
               break;

            auto filter = convert_text_to_filter(sstream);
            if (filter)
               filters.push_back(filter);
         }

         return filters;
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<or_tree_filter_t>(wistringstream& sstream)
      {
         auto filters = convert_text_to_multiple_filters(sstream);

         eat_closing_brace(sstream);

         return any(filters);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<and_tree_filter_t>(wistringstream& sstream)
      {
         auto filters = convert_text_to_multiple_filters(sstream);

         eat_closing_brace(sstream);

         return all(filters);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<under_tree_filter_t>(wistringstream& sstream)
      {
         bool include_self;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> include_self >> skipws >> comma;

         auto filter = convert_text_to_filter(sstream);

         eat_closing_brace(sstream);

         return under(filter, include_self);
      }

      template <>
      tree_filter_ptr_t convert_text_to_filter<remove_children_tree_filter_t>(wistringstream& sstream)
      {
         bool remove_self;
         wchar_t comma;
         sstream >> skipws >> boolalpha >> remove_self >> skipws >> comma;

         auto filter = convert_text_to_filter(sstream);

         eat_closing_brace(sstream);

         return no_child(filter, remove_self);
      }

      template<>
      tree_filter_ptr_t convert_text_to_filter<level_range_tree_filter_t>(wistringstream& sstream)
      {
         size_t min_level, max_level;
         wchar_t comma;
         sstream >> skipws >> min_level >> skipws >> comma >> skipws >> max_level;

         eat_closing_brace(sstream);

         return level_range(min_level, max_level);
      }

      template<>
      tree_filter_ptr_t convert_text_to_filter<named_tree_filter_t>(wistringstream& sstream)
      {
         wstring name;
         sstream >> skipws >> quoted(name);

         eat_closing_brace(sstream);

         auto named = make_shared<named_tree_filter_t>();
         named->name = name;
         return named;
      }

      tree_filter_ptr_t convert_text_to_filter(wistringstream& sstream)
      {
         #define CALL_CONVERTER(a,b) if (name == a) { return convert_text_to_filter<b>(sstream); } else

         wstring name;
         wchar_t brace;
         sstream >> skipws >> name >> skipws >> brace;

         CALL_CONVERTER(L"accept", accept_tree_filter_t)
         CALL_CONVERTER(L"contains", contains_tree_filter_t)
         CALL_CONVERTER(L"unique", unique_tree_filter_t)
         CALL_CONVERTER(L"regex", regex_tree_filter_t)
         CALL_CONVERTER(L"not", not_tree_filter_t)
         CALL_CONVERTER(L"if-sub", if_subtree_tree_filter_t)
         CALL_CONVERTER(L"if-sib", if_sibling_tree_filter_t)
         CALL_CONVERTER(L"or", or_tree_filter_t)
         CALL_CONVERTER(L"and", and_tree_filter_t)
         CALL_CONVERTER(L"under", under_tree_filter_t)
         CALL_CONVERTER(L"no-child", remove_children_tree_filter_t)
         CALL_CONVERTER(L"range", level_range_tree_filter_t)
         CALL_CONVERTER(L"stop", stop_tree_filter_t)
         CALL_CONVERTER(L"stop-when-kept", stop_when_kept_tree_filter_t)
         CALL_CONVERTER(L"until", until_tree_filter_t)
         CALL_CONVERTER(L"named", named_tree_filter_t)

         #undef CALL_CONVERTER

         return {};
      }

      tree_filter_ptr_t convert_text_to_filter(wistringstream& sstream, const named_filters_t& named)
      {
         tree_filter_ptr_t filter = convert_text_to_filter(sstream);
         update_named_filters(filter, named);
         return filter;
      }
   }

   wstring convert_filter_to_text(const tree_filter_ptr_t& filter)
   {
      return V1::convert_filter_to_text(filter);
   }

   tree_filter_ptr_t convert_text_to_filter(const wstring& text, const named_filters_t& named)
   {
      if (text.find(L"V1: ") == 0)
      {
         wistringstream sstream(text.substr(4));
         return V1::convert_text_to_filter(sstream, named);
      }

      return {};
   }
}
